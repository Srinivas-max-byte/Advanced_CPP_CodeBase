# Client-Server Socket Programming - Complete Guide

## Overview

This guide covers both the **server** (`simpleServer.cpp`) and **client** (`simpleClient.cpp`) implementations for TCP socket communication.

---

## Server vs Client Architecture

```
????????????????????????????????????????
?           SERVER                     ?
?  (simpleServer.cpp)                  ?
????????????????????????????????????????
? 1. socket()   - Create socket        ?
? 2. bind()     - Bind to port 8080    ?
? 3. listen()   - Listen for clients   ?
? 4. accept()   - Accept connection    ?
? 5. recv()     - Receive message      ?
? 6. send()     - Send response        ?
? 7. close()    - Close connection     ?
????????????????????????????????????????
              ? TCP Connection
????????????????????????????????????????
?           CLIENT                     ?
?  (simpleClient.cpp)                  ?
????????????????????????????????????????
? 1. socket()   - Create socket        ?
? 2. connect()  - Connect to server    ?
? 3. send()     - Send message         ?
? 4. recv()     - Receive response     ?
? 5. close()    - Close connection     ?
????????????????????????????????????????
```

---

## Client Code Breakdown (simpleClient.cpp)

### **Step 1: Create a Socket**

```cpp
int client_socket = socket(AF_INET, SOCK_STREAM, 0);

if (client_socket < 0) {
    cerr << "Error: Failed to create socket" << endl;
    return 1;
}
```

**What it does:**
- Creates a TCP socket using IPv4
- Returns a file descriptor (socket handle) if successful
- Returns -1 on error

**Same as server:** The socket creation is identical to the server

---

### **Step 2: Define Server Address**

```cpp
struct sockaddr_in server_address;
memset(&server_address, 0, sizeof(server_address));

server_address.sin_family = AF_INET;
server_address.sin_port = htons(SERVER_PORT);  // Port 8080
```

**What it does:**
- Creates a structure to hold the server's address information
- Sets address family to IPv4 (AF_INET)
- Sets the port number (must match server's port)

**Key difference from server:**
- Server uses `INADDR_ANY` (listens on all interfaces)
- Client specifies exact server IP address

---

### **Step 3: Convert and Set Server IP Address**

```cpp
if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
    cerr << "Error: Invalid server IP address" << endl;
    close(client_socket);
    return 1;
}
```

**What it does:**
- Converts IP address from string ("127.0.0.1") to network binary format
- `inet_pton()` = "Presentation to Network"
- Returns 1 on success, 0 for invalid format, -1 for error

**Why this instead of htonl(INADDR_ANY)?**
- Server uses `htonl(INADDR_ANY)` because it accepts any interface
- Client needs to connect to specific IP, so we use `inet_pton()`

---

### **Step 4: Connect to Server** ? **KEY DIFFERENCE FROM SERVER**

```cpp
if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    cerr << "Error: Failed to connect to server" << endl;
    close(client_socket);
    return 1;
}

cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << endl;
```

**What it does:**
- Establishes TCP connection with the server
- Performs 3-way handshake (SYN, SYN-ACK, ACK)
- Blocks until connection is established or fails

**Parameters:**
- `client_socket`: Socket to use
- `&server_address`: Server address to connect to
- `sizeof(server_address)`: Size of address structure

**Return value:**
- 0 on success
- -1 on error

**Server doesn't do this:**
- Server calls `bind()` and `listen()` instead
- Server waits for clients to connect via `accept()`

---

### **Step 5: Send Message to Server**

```cpp
const char* message = "Hello from client!";
ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);

if (bytes_sent < 0) {
    cerr << "Error: Failed to send message" << endl;
    close(client_socket);
    return 1;
}

cout << "Message sent to server: " << message << endl;
```

**What it does:**
- Sends data through the connected socket to the server
- Automatically handled by TCP (segmentation, ordering, reliability)

**Parameters:**
- `client_socket`: Connected socket
- `message`: Data to send
- `strlen(message)`: Number of bytes to send
- `0`: Flags (standard send)

**Return value:**
- Number of bytes sent (usually equals requested bytes for small messages)
- -1 on error

---

### **Step 6: Receive Response from Server**

```cpp
char buffer[BUFFER_SIZE];
memset(buffer, 0, BUFFER_SIZE);

ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

if (bytes_received > 0) {
    buffer[bytes_received] = '\0';
    cout << "Response from server: " << buffer << endl;
} else if (bytes_received == 0) {
    cout << "Server closed the connection" << endl;
} else {
    cerr << "Error: Failed to receive data" << endl;
}
```

**What it does:**
- Receives data sent by the server
- Blocks until data arrives or connection closes

**Parameters:**
- `client_socket`: Connected socket
- `buffer`: Where to store received data
- `BUFFER_SIZE - 1`: Max bytes (reserve 1 for null terminator)
- `0`: Flags (standard recv)

**Return value:**
- Number of bytes received (>0)
- 0 if server closed connection
- -1 on error

---

### **Step 7: Close Connection**

```cpp
close(client_socket);
cout << "Connection closed" << endl;
```

**What it does:**
- Closes the socket and releases resources
- Sends FIN packet to server (graceful close)

---

## How Server and Client Work Together

### **Connection Flow Diagram**

```
CLIENT                                SERVER

socket()                              socket()
                                      bind()
                                      listen()
connect() [SYN] ???????????????????  accept() [waiting]
                                      [SYN-ACK] ??????????
                 [ACK] ???????????????
                                      [accept() returns]
         ? Connected

send("Hello") ???????????????????     recv()
                                      [Receives "Hello"]
                                      send("Message received...")
                 ??????????????????     [Sends response]

recv() ?????????????????????????      
[Receives "Message received..."]

close() ?????????????????????????     close()
        [FIN] ????????????????????????? [FIN-ACK]
              ????????????????????????? [ACK]
? Closed                               ? Closed
```

---

## Running the Programs

### **On Linux/Mac:**

**Terminal 1 - Start the Server:**
```bash
cd Networking
g++ -std=c++11 simpleServer.cpp -o simpleServer
./simpleServer
```

Output:
```
Socket created successfully
Socket bound to port 8080
Server listening on port 8080...
```

The server now waits for clients to connect.

**Terminal 2 - Run the Client:**
```bash
cd Networking
g++ -std=c++11 simpleClient.cpp -o simpleClient
./simpleClient
```

Output:
```
Socket created successfully
Connected to server at 127.0.0.1:8080
Message sent to server: Hello from client!
Bytes sent: 18
Response from server: Message received by server!
Bytes received: 28
Connection closed
```

**Back in Terminal 1 - Server Output:**
```
Socket created successfully
Socket bound to port 8080
Server listening on port 8080...

Client connected from 127.0.0.1:12345
Received from client: Hello from client!
Response sent to client
```

---

## Key Differences: Server vs Client

| Aspect | Server | Client |
|--------|--------|--------|
| **Socket Creation** | Same | Same |
| **Address Setup** | `INADDR_ANY` (all interfaces) | Specific IP address |
| **Address Format** | `htonl(INADDR_ANY)` | `inet_pton()` |
| **Socket Role** | Passive (listening) | Active (connecting) |
| **bind()** | ? Required | ? Not needed |
| **listen()** | ? Required | ? Not needed |
| **connect()** | ? Not needed | ? Required |
| **accept()** | ? Required | ? Not needed |
| **recv/send** | Works after accept() | Works after connect() |
| **Lifetime** | Infinite loop (accepts multiple clients) | Single transaction, then exits |

---

## Code Modifications for Different Scenarios

### **Scenario 1: Connect to Remote Server**

Change in `simpleClient.cpp`:
```cpp
const char* SERVER_IP = "192.168.1.100";  // Change to server's IP
const int SERVER_PORT = 8080;              // Match server's port
```

### **Scenario 2: Send Different Message**

Change in `simpleClient.cpp`:
```cpp
const char* message = "Your custom message here!";
```

### **Scenario 3: Receive Larger Response**

Change in `simpleClient.cpp`:
```cpp
const int BUFFER_SIZE = 4096;  // Increase buffer size
```

### **Scenario 4: Multiple Interactions**

Modify `simpleClient.cpp` to loop:
```cpp
for (int i = 0; i < 5; i++) {  // Send 5 messages
    const char* message = "Message " + to_string(i);
    send(client_socket, message, strlen(message), 0);
    
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    cout << "Response: " << buffer << endl;
}
```

### **Scenario 5: Handle Multiple Clients (Server)**

The current server handles one client at a time. For concurrent clients, use threading:

```cpp
#include <thread>

while (true) {
    int client_socket = accept(server_socket, ...);
    
    // Create thread to handle this client
    thread t([client_socket]() {
        recv(client_socket, ...);
        send(client_socket, ...);
        close(client_socket);
    });
    t.detach();  // Thread runs independently
}
```

---

## Common Issues and Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| Client: "Connection refused" | Server not running or wrong port | Start server first, check port matches |
| Client: "Invalid server IP address" | Wrong IP format | Use "127.0.0.1" for localhost or correct IP |
| Server: "Address already in use" | Previous instance still holds port | Wait 30-60 seconds or use SO_REUSEADDR |
| No output | Program crashed silently | Check return values and error messages |
| Partial messages received | TCP doesn't guarantee recv() gets all data | Loop recv() until all data received |
| Server exits after one client | Infinite loop broken | Check if close() is inside or outside while loop |

---

## Advanced Concepts

### **Socket States in TCP**

```
CLIENT SIDE:
CLOSED ? SYN_SENT ? ESTABLISHED ? FIN_WAIT1 ? CLOSED
                          ?
                    (send/recv)

SERVER SIDE:
LISTEN ? SYN_RECEIVED ? ESTABLISHED ? CLOSE_WAIT ? CLOSED
                              ?
                          (send/recv)
```

### **What TCP Guarantees**

? **Delivery:** All bytes arrive (no loss)
? **Ordering:** Bytes arrive in sent order
? **No Duplicates:** Each byte delivered once
? **Error Detection:** Corrupted packets detected and retransmitted

? **Message Boundaries:** TCP is a stream, not a message service
- If client sends "ABC", server might receive "A", then "BC"
- You must handle this in your application layer

---

## Summary

- **Server:** Creates socket ? Binds to port ? Listens ? Accepts connections ? Communicates
- **Client:** Creates socket ? Connects to server ? Sends/Receives ? Closes
- **Connection:** TCP handles all the complex synchronization and reliability
- **Your Code:** Simple and demonstrates the fundamental client-server pattern

Both files are in the `Networking/` directory and ready to compile and run!
