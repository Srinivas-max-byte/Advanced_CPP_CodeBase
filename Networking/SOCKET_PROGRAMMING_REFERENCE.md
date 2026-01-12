# Socket Programming Reference Guide

## Table of Contents
1. [struct sockaddr_in Documentation](#struct-sockaddrin-documentation)
2. [sockaddr_in vs sockaddr Comparison](#sockaddrin-vs-sockaddr-comparison)
3. [Byte Order Conversion Functions](#byte-order-conversion-functions)
4. [sin_addr.s_addr Values](#sinaddrs_addr-values)
5. [Socket Creation Steps - Detailed Guide](#socket-creation-steps---detailed-guide)
6. [Key Concepts and Explanations](#key-concepts-and-explanations)

---

## struct sockaddr_in Documentation

### Definition
The `sockaddr_in` structure is used in C++ to define an Internet address for socket programming, specifically for IPv4 addresses, encapsulating the address family, port number, and IP address.

### Member Fields
```cpp
struct sockaddr_in {
    sa_family_t sin_family;    // Address family (AF_INET for IPv4)
    in_port_t sin_port;        // Port number (16-bit)
    struct in_addr sin_addr;   // IPv4 address (32-bit)
    char sin_zero[8];          // Padding to match sizeof(struct sockaddr)
};
```

### Field Explanations
- **sin_family**: Specifies the address family (IPv4 in this case using AF_INET)
- **sin_port**: Port number where the server will listen
- **sin_addr.s_addr**: IPv4 address (0.0.0.0, 127.0.0.1, or specific IP)
- **sin_zero**: Unused padding bytes

---

## sockaddr_in vs sockaddr Comparison

### Key Differences

| Aspect | sockaddr | sockaddr_in |
|--------|----------|------------|
| **Purpose** | Generic socket address structure (protocol-agnostic) | IPv4-specific address structure |
| **Size** | Smaller (16 bytes minimum) | Larger (16 bytes, optimized for IPv4) |
| **Fields** | `sa_family` + `sa_data[14]` (generic data array) | `sin_family`, `sin_port`, `sin_addr`, `sin_zero` |
| **Type Safety** | Less type-safe (raw data array) | More type-safe (named fields) |

### Why Use struct sockaddr in bind()?

The `bind()` function signature requires `struct sockaddr*`:
```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Reasons:**
1. **Protocol Independence**: The function can work with any protocol family:
   - IPv4 (`sockaddr_in`)
   - IPv6 (`sockaddr_in6`)
   - UNIX sockets (`sockaddr_un`)
   - Others

2. **Type Casting Bridge**: 
   ```cpp
   bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))
   ```
   - Cast `sockaddr_in*` to `sockaddr*`
   - Works because both structures have compatible memory layout
   - The `sa_family` field identifies the protocol type

3. **Backward Compatibility**: Maintains stable API across different address families

### Memory Layout Analogy
- **`sockaddr`** = Generic envelope (just needs to know the type)
- **`sockaddr_in`** = IPv4-specific envelope with labeled compartments for port, address, etc.

The kernel checks the `sa_family` field to properly interpret the remaining bytes.

---

## Byte Order Conversion Functions

### What is the Byte Order Problem?

Different computer architectures store multi-byte integers differently:

- **Little-endian** (Intel, AMD): Least significant byte stored first
  - Example: `0x12345678` stored as `78 56 34 12` in memory
  
- **Big-endian** (Network byte order): Most significant byte stored first
  - Example: `0x12345678` stored as `12 34 56 78` in memory

**Networks use big-endian (network byte order) as the standard.**

### htonl() - "Host to Network Long"

**Purpose**: Converts a 32-bit (4-byte) IP address from host byte order to network byte order

```cpp
server_address.sin_addr.s_addr = htonl(INADDR_ANY);
```

- `h` = host
- `n` = network
- `to` = convert to
- `l` = long (32-bit)

**Input**: Host byte order (whatever your machine uses)
**Output**: Network byte order (big-endian, required for transmission)

### htons() - "Host to Network Short"

**Purpose**: Converts a 16-bit (2-byte) port number from host byte order to network byte order

```cpp
server_address.sin_port = htons(PORT);  // PORT = 8080
```

- `h` = host
- `n` = network
- `to` = convert to
- `s` = short (16-bit)

**Input**: Host byte order
**Output**: Network byte order

### Practical Example

```cpp
int port = 8080;  // Host byte order (decimal)

// WITHOUT htonl() - WRONG (on little-endian machine):
// Port 8080 = 0x1F90 in hex
// Little-endian stores as: 90 1F (backwards!)
// Network receives: 0x1F90 → interpreted as port 8080 in big-endian = 7952 (WRONG!)

// WITH htons() - CORRECT:
// Converts to big-endian: 1F 90
// Network receives: 0x1F90 → correctly interpreted as port 8080
server_address.sin_port = htons(8080);
```

### Inverse Functions

| Function | Purpose |
|----------|---------|
| `htonl()` | Host → Network (32-bit) |
| `htons()` | Host → Network (16-bit) |
| `ntohl()` | Network → Host (32-bit) |
| `ntohs()` | Network → Host (16-bit) |

**Usage in Code**:
```cpp
// When receiving from a client - convert back to host order for display
int client_port = ntohs(client_address.sin_port);
```

### Key Takeaway

These functions ensure **communication compatibility** across machines with different architectures. Without them:
- A server on a little-endian machine
- A client on a big-endian machine
- Would have garbled IP addresses and ports

---

## sin_addr.s_addr Values

### Common Values

| Value | Purpose | Connection Source |
|-------|---------|-------------------|
| `INADDR_ANY` | Accept connections on **any available IPv4 interface** (0.0.0.0) | All interfaces |
| `INADDR_LOOPBACK` | Accept only **localhost connections** (127.0.0.1) | Localhost only |
| Specific IP address | Accept connections on a **specific IPv4 address** | That specific interface only |
| `INADDR_BROADCAST` | Broadcast address (255.255.255.255) | Not typical for bind() |

### Implementation Examples

#### 1. INADDR_ANY (Your Current Code)
```cpp
server_address.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0
```
- Server listens on **all available network interfaces**
- Clients can connect from any interface (Ethernet, WiFi, etc.)
- **Most common for multi-interface systems**

#### 2. INADDR_LOOPBACK
```cpp
server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1
```
- Server listens **only on localhost**
- Only connections from the **same machine** can connect
- Useful for debugging or internal testing

#### 3. Specific IPv4 Address Using inet_pton() - RECOMMENDED
```cpp
struct sockaddr_in server_address;
inet_pton(AF_INET, "192.168.1.100", &server_address.sin_addr);
// No htonl() needed; inet_pton() converts to network byte order automatically
```
- Server listens **only on the specified IP address**
- Example: `192.168.1.100`
- Modern, portable approach
- Better for IPv6 support

#### 4. Specific IPv4 Address Using inet_aton() - LEGACY
```cpp
inet_aton("192.168.1.100", &server_address.sin_addr);
// Older function; inet_pton() is preferred
```

#### 5. INADDR_BROADCAST
```cpp
server_address.sin_addr.s_addr = htonl(INADDR_BROADCAST);  // 255.255.255.255
```
- Not typically used for `bind()`
- Used more for **broadcast UDP packets**

### Comparison Summary

| Address | Value | Use Case |
|---------|-------|----------|
| `INADDR_ANY` | 0.0.0.0 | Accept from all interfaces (default/general-purpose) |
| `INADDR_LOOPBACK` | 127.0.0.1 | Local testing/debugging only |
| `192.168.1.100` | Specific IP | Bind to specific interface only |

### Modern Best Practice

Use `inet_pton()` for better portability and IPv6 support:

```cpp
if (inet_pton(AF_INET, "0.0.0.0", &server_address.sin_addr) <= 0) {
    cerr << "Invalid address" << endl;
    return 1;
}
```

The `inet_pton()` function automatically handles byte order conversion, so you don't need `htonl()`.

---

## Socket Creation Steps - Detailed Guide

### Overview

Creating a TCP server socket involves several sequential steps. Each step prepares the socket for the next operation. The typical flow is:

```
socket() → bind() → listen() → accept() → recv()/send() → close()
```

---

### Step 1: socket() - Create a Socket Endpoint

#### What It Does
Creates a new socket - an endpoint for network communication. Think of it as creating an empty mailbox.

#### Syntax
```cpp
int socket(int domain, int type, int protocol);
```

#### Parameters
- **domain**: Address family
  - `AF_INET` = IPv4
  - `AF_INET6` = IPv6
  - `AF_UNIX` = Unix domain sockets
  
- **type**: Communication type
  - `SOCK_STREAM` = TCP (connection-oriented, reliable)
  - `SOCK_DGRAM` = UDP (connectionless, unreliable)
  
- **protocol**: Specific protocol
  - `0` = Let system choose (recommended for `SOCK_STREAM`)
  - `IPPROTO_TCP` = Explicitly choose TCP
  - `IPPROTO_UDP` = Explicitly choose UDP

#### Return Value
- **Success**: Non-negative file descriptor (socket handle)
- **Failure**: `-1` (error)

#### Example Code
```cpp
int server_socket = socket(AF_INET, SOCK_STREAM, 0);

if (server_socket < 0) {
    cerr << "Error: Failed to create socket" << endl;
    return 1;
}

cout << "Socket created successfully" << endl;
```

#### What Happens Internally
1. Kernel allocates a socket data structure
2. Associates the socket with the specified address family and protocol
3. Returns a file descriptor that represents this socket
4. Socket is created but **not yet bound to any address or port**
5. Socket is **not yet listening for connections**

#### State After socket()
```
┌─────────────────────┐
│   Socket Created    │
│  - No address       │
│  - No port          │
│  - Not listening    │
└─────────────────────┘
```

---

### Step 2: bind() - Associate Socket with Address and Port

#### What It Does
Binds the socket to a specific IP address and port number. This tells the OS "when data arrives on this address and port, deliver it to this socket."

#### Syntax
```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

#### Parameters
- **sockfd**: Socket file descriptor (from `socket()`)
- **addr**: Pointer to `sockaddr_in` structure (cast as `sockaddr*`)
- **addrlen**: Size of the address structure

#### Return Value
- **Success**: `0`
- **Failure**: `-1` (error)

#### Example Code
```cpp
struct sockaddr_in server_address;
memset(&server_address, 0, sizeof(server_address));

server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept on any interface
server_address.sin_port = htons(8080);               // Port 8080

if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    cerr << "Error: Failed to bind socket to port 8080" << endl;
    close(server_socket);
    return 1;
}

cout << "Socket bound to port 8080" << endl;
```

#### What Happens Internally
1. Kernel checks if the port is already in use
2. If port is free, kernel associates the socket with that address:port combination
3. Kernel creates a binding table entry mapping: `IP address:port → socket file descriptor`
4. Any packets arriving at this address:port are now routed to this socket
5. If another process tries to bind to the same address:port, it will fail

#### Common Error: "Address already in use"
```cpp
// This error occurs when:
// - Port 8080 is already in use by another process
// - A previous process hasn't fully released the port (TIME_WAIT state)

// Solution: Use SO_REUSEADDR socket option
int opt = 1;
setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
```

#### State After bind()
```
┌─────────────────────────┐
│   Socket Created        │
│   Socket Bound          │
│  - Address: 0.0.0.0     │
│  - Port: 8080           │
│  - Not listening yet    │
└─────────────────────────┘
```

#### Why Cast sockaddr_in to sockaddr?
```cpp
// sockaddr_in is protocol-specific (IPv4)
struct sockaddr_in server_address;

// bind() accepts generic sockaddr* for protocol independence
// It uses sa_family field to determine the actual structure type
bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))
//                   ↑ Cast to generic pointer
```
---

### Step 3: listen() - Put Socket in Listening Mode

#### What It Does
Marks the socket as a passive socket that **listens for incoming connections**. 
It creates a queue to hold pending connection requests while the server is busy handling other clients.

#### Syntax
```cpp
int listen(int sockfd, int backlog);
```

#### Parameters
- **sockfd**: Socket file descriptor (from `socket()`)
- **backlog**: Maximum number of pending connection requests in the queue
  - Typical values: 5-128
  - If queue is full, new connection attempts are rejected
  - OS may allow a slightly higher number than specified

#### Return Value
- **Success**: `0`
- **Failure**: `-1` (error)

#### Example Code
```cpp
const int BACKLOG = 5;

if (listen(server_socket, BACKLOG) < 0) {
    cerr << "Error: Failed to listen on socket" << endl;
    close(server_socket);
    return 1;
}

cout << "Server listening on port 8080..." << endl;
```

#### What Happens Internally
1. Kernel marks the socket as a **listening socket** (passive)
2. OS allocates a **queue** for pending connections
3. Queue size = `backlog` parameter
4. When clients try to connect, they are placed in this queue
5. Server retrieves clients one-by-one using `accept()`

#### Connection Queue Visualization
```
Incoming Connection Requests
         ↓
    ┌────────┐
    │ Queue  │ ← backlog = 5 (max 5 pending connections)
    │ [1]    │
    │ [2]    │
    │ [3]    │
    │ [4]    │
    │ [5]    │ ← Full! New connections rejected
    └────────┘
         ↓
    accept() → Retrieves one client from queue
```

#### What Happens When Backlog is Full?
```cpp
// If 5 connections are already queued and a 6th client tries to connect:
// - Connection is rejected with RST (reset) packet
// - Client sees "Connection refused" error
// - Server should process accept() faster to avoid this
```

#### backlog Parameter Behavior
```cpp
// Scenario 1: Small backlog (not recommended)
listen(server_socket, 1);  // Only 1 pending connection allowed
// Problem: High-traffic servers will reject many connections

// Scenario 2: Reasonable backlog
listen(server_socket, 5);  // 5 pending connections (common for low-traffic)
// Good for simple applications

// Scenario 3: Large backlog
listen(server_socket, 128);  // 128 pending connections (high-traffic servers)
// Better for servers expecting many simultaneous connection attempts
```

#### State After listen()
```
┌──────────────────────┐
│   Socket Created     │
│   Socket Bound       │
│   Socket Listening   │
│  - Address: 0.0.0.0  │
│  - Port: 8080        │
│  - Queue: empty      │
│  - Backlog: 5        │
└──────────────────────┘
         ↓
    Waiting for clients...
```

---

### Step 4: accept() - Accept Incoming Client Connections

#### What It Does
Accepts an incoming connection from a client and creates a **new socket** for communicating with that client. 
The original listening socket remains open for accepting more clients.

#### Syntax
```cpp
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

#### Parameters
- **sockfd**: Listening socket file descriptor (from `listen()`)
- **addr**: Pointer to `sockaddr_in` structure (will be filled with client's address info)
- **addrlen**: Pointer to size of address structure (will be updated with actual size)

#### Return Value
- **Success**: New socket file descriptor for communicating with the client
- **Failure**: `-1` (error)

#### Example Code
```cpp
while (true) {
    // Create structure to hold client information
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    
    // Accept a connection from a waiting client
    int client_socket = accept(server_socket, 
                               (struct sockaddr*)&client_address, 
                               &client_address_len);
    
    if (client_socket < 0) {
        cerr << "Error: Failed to accept connection" << endl;
        continue;
    }
    
    // Extract client information
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_address.sin_port);
    
    cout << "Client connected from " << client_ip << ":" << client_port << endl;
    
    // Now use client_socket for communication with this specific client
    // Original server_socket continues listening for more clients
}
```
#### What Happens Internally
1. If queue is empty: `accept()` **blocks** (waits) until a client connects
2. If queue has pending connections: `accept()` immediately returns
3. Kernel removes first connection from the queue
4. Creates a **new socket** for this client connection
5. Fills the `client_address` structure with client's IP and port
6. Returns the file descriptor of the new socket
7. Original `server_socket` is unaffected and continues listening

#### Key Points: Two Different Sockets

```cpp
// LISTENING SOCKET (from socket() + bind() + listen())
int server_socket = socket(...);
bind(server_socket, ...);
listen(server_socket, ...);
// Purpose: Listen for incoming connections
// Use: Only for accept()
// Lifecycle: Stays open throughout server lifetime

// CLIENT SOCKET (from accept())
int client_socket = accept(server_socket, ...);
// Purpose: Communicate with one specific client
// Use: For send(), recv(), close()
// Lifecycle: Created per client, closed when done
```
---

#### Multiple Clients Handling

```cpp
// Server accepts multiple clients sequentially
while (true) {
    int client_socket = accept(server_socket, ...);  // Get new client socket
    
    // Communicate with this client
    recv(client_socket, ...);
    send(client_socket, ...);
    close(client_socket);  // Close this client connection
    
    // server_socket is still open! Ready for next client
}

// For true concurrent clients, use threading:
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

#### accept() Blocking Behavior

```cpp
// BLOCKING (default)
int client_socket = accept(server_socket, ...);
// Waits indefinitely until a client connects
// Program execution pauses here

// NON-BLOCKING (using fcntl)
fcntl(server_socket, F_SETFL, O_NONBLOCK);
int client_socket = accept(server_socket, ...);
// Returns immediately even if no client is waiting
// Returns -1 with errno = EAGAIN if queue is empty
```
---

#### State After accept()
```
┌──────────────────────┐
│   Server Socket      │  ← Still listening!
│  - Listening         │     Can accept more clients
└──────────────────────┘
         ↓
┌──────────────────────┐
│   Client Socket 1    │  ← New socket for client 1
│  - Connected         │     Can send/recv data
└──────────────────────┘
         ↓
┌──────────────────────┐
│   Client Socket 2    │  ← New socket for client 2
│  - Connected         │     Can send/recv data
└──────────────────────┘
```
---

### Step 5: recv() and send() - Communicate with Client

#### recv() - Receive Data from Client

```cpp
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

- **sockfd**: Client socket (from `accept()`)
- **buf**: Buffer to store received data
- **len**: Maximum bytes to read
- **flags**: Options (usually 0)
- **Returns**: Number of bytes received (0 if client disconnected, -1 if error)

#### send() - Send Data to Client

```cpp
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

- **sockfd**: Client socket (from `accept()`)
- **buf**: Data to send
- **len**: Number of bytes to send
- **flags**: Options (usually 0)
- **Returns**: Number of bytes sent (-1 if error)

#### Example
```cpp
// Receive from client
char buffer[1024];
ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

if (bytes_received > 0) {
    buffer[bytes_received] = '\0';  // Null-terminate
    cout << "Received: " << buffer << endl;
    
    // Send response back
    const char* response = "Message received!";
    send(client_socket, response, strlen(response), 0);
}
else if (bytes_received == 0) {
    cout << "Client disconnected" << endl;
}
```
---

### Step 6: close() - Close Sockets

#### Close Client Socket
```cpp
close(client_socket);  // Closes connection with this specific client
```
- Sends FIN (finish) packet to client
- Releases kernel resources for this connection
- Original `server_socket` remains open

#### Close Server Socket
```cpp
close(server_socket);  // Stops listening for new connections
```
- Stops accepting new connections
- Closes the listening socket
- Should be done when server shuts down

---

### Complete Server Workflow Diagram

```
┌─────────────────────────────────────────────────────┐
│ Step 1: socket()                                    │
│ Create socket endpoint                              │
│ Returns: server_socket file descriptor              │
└─────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────┐
│ Step 2: bind()                                      │
│ Bind socket to IP address:port (0.0.0.0:8080)       │
│ Returns: 0 (success) or -1 (error)                  │
└─────────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────────┐
│ Step 3: listen()                                    │
│ Put socket in listening mode, create connection     │
│ queue with backlog=5                                │
│ Returns: 0 (success) or -1 (error)                  │
└─────────────────────────────────────────────────────┘
                    ↓
         ⚡ Server Ready! ⚡
                    ↓
┌─────────────────────────────────────────────────────┐
│ Step 4: accept() [LOOP]                             │
│ Wait for client connection                          │
│ Returns: client_socket file descriptor              │
└─────────────────────────────────────────────────────┘
                    ↓
      ┌─────────────┴──────────────┐
      ↓                            ↓
┌──────────────-┐          ┌──────────────-┐
│ Step 5: recv()│          │ Step 5: send()│
│ Receive data  │          │ Send response │
└──────────────-┘          └──────────────-┘
      ↓                            ↓
      └─────────────┬──────────────┘
                    ↓
┌─────────────────────────────────────────────────────┐
│ Step 6: close(client_socket)                        │
│ Close client connection                             │
│ (server_socket stays open for next client)          │
└─────────────────────────────────────────────────────┘
                    ↓
         Loop back to Step 4
    (accept next client connection)
                    ↓
            [Server shutdown]
                    ↓
┌─────────────────────────────────────────────────────┐
│ Final: close(server_socket)                         │
│ Close listening socket                              │
└─────────────────────────────────────────────────────┘
```
---

## Key Concepts and Explanations

### sin_family Full Form
**sin_family** = **"socket internet family"**

The naming convention for `sockaddr_in` members:
- **`sin_`** = socket internet (prefix for members of `sockaddr_in`)
- **`family`** = the address family field

Other members follow the same pattern:
- `sin_addr` = socket internet address
- `sin_port` = socket internet port

### Socket Creation Parameters

```cpp
int server_socket = socket(AF_INET, SOCK_STREAM, 0);
```

- **AF_INET**: IPv4 address family
- **SOCK_STREAM**: TCP protocol (connection-oriented, reliable, ordered delivery)
- **0**: Let the system choose the appropriate protocol (IPPROTO_TCP for SOCK_STREAM)

**Returns**: File descriptor (socket handle) if successful, or -1 on error

### Server Workflow

1. **Create Socket**: `socket()` → creates endpoint for communication
2. **Define Address**: `sockaddr_in` → specifies address and port
3. **Bind**: `bind()` → associates socket with address and port
4. **Listen**: `listen()` → puts socket in listening mode
5. **Accept**: `accept()` → accepts incoming client connections
6. **Communicate**: `recv()` and `send()` → exchange data with client
7. **Close**: `close()` → cleanup

### BACKLOG Parameter

```cpp
listen(server_socket, BACKLOG);
```

- **BACKLOG = 5**: Maximum number of queued connections
- When a client tries to connect and the server is busy, the connection is queued
- Once the queue is full, new connections are refused

---

## Quick Reference Code Snippets

### Complete Server Setup
```cpp
// 1. Create socket
int server_socket = socket(AF_INET, SOCK_STREAM, 0);
if (server_socket < 0) { perror("socket"); exit(1); }

// 2. Define address structure
struct sockaddr_in server_address;
memset(&server_address, 0, sizeof(server_address));
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = htonl(INADDR_ANY);
server_address.sin_port = htons(8080);

// 3. Bind socket
if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    perror("bind");
    exit(1);
}

// 4. Listen for connections
if (listen(server_socket, 5) < 0) {
    perror("listen");
    exit(1);
}

cout << "Server listening on port 8080..." << endl;

// 5. Accept and communicate
while (true) {
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    
    int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
    if (client_socket < 0) { perror("accept"); continue; }
    
    char buffer[1024];
    ssize_t bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        cout << "Received: " << buffer << endl;
        send(client_socket, "OK", 2, 0);
    }
    
    close(client_socket);
}

close(server_socket);
```

### Initialize Server Address Structure
```cpp
struct sockaddr_in server_address;
memset(&server_address, 0, sizeof(server_address));
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = htonl(INADDR_ANY);
server_address.sin_port = htons(8080);
```

### Bind Socket
```cpp
if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    cerr << "Error: Failed to bind socket" << endl;
    close(server_socket);
    return 1;
}
```

### Accept Client Connection
```cpp
struct sockaddr_in client_address;
socklen_t client_address_len = sizeof(client_address);
int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
```

### Get Client Information
```cpp
char client_ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
int client_port = ntohs(client_address.sin_port);
cout << "Client connected from " << client_ip << ":" << client_port << endl;
```

---

## Common Issues and Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| Port already in use | Previous process still holds port | Wait or use SO_REUSEADDR option |
| Connection refused | Server not listening on that address/port | Verify address and port binding |
| Garbled data | Byte order mismatch | Always use htons()/htonl() and ntohs()/ntohl() |
| Client can't find server | Server on INADDR_LOOPBACK | Use INADDR_ANY instead |
| accept() hangs | No clients connecting / blocking mode | Check firewall, use non-blocking mode |
| recv() returns 0 | Client disconnected | Check for connection close and handle gracefully |
