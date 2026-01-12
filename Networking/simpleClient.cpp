#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    const char* SERVER_IP = "127.0.0.1";  // Server address (localhost)
    const int SERVER_PORT = 8080;          // Must match server port
    const int BUFFER_SIZE = 1024;
    
    // Create a socket
    // socket() parameters:
    //   - AF_INET: IPv4 address family
    //   - SOCK_STREAM: TCP protocol (connection-oriented, reliable, ordered delivery)
    //   - 0: Let the system choose the appropriate protocol (IPPROTO_TCP for SOCK_STREAM)
    // Returns a file descriptor (socket handle) if successful, or -1 on error
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (client_socket < 0) {
        cerr << "Error: Failed to create socket" << endl;
        return 1;
    }
    
    cout << "Socket created successfully" << endl;
    
    // Define server address structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    
    // sin_family specifies the address family (IPv4 in this case using AF_INET)
    server_address.sin_family = AF_INET;
    
    // Convert server IP address from string format to binary format
    // inet_pton() converts presentation format (string like "127.0.0.1") to network format (binary)
    // AF_INET: IPv4 address family
    // SERVER_IP: String representation of IP address
    // &server_address.sin_addr: Where to store the binary IP address
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        cerr << "Error: Invalid server IP address" << endl;
        close(client_socket);
        return 1;
    }
    
    // sin_port specifies the port number where the server is listening
    // htons() "Host to Network Short" converts from host byte order to network byte order (big-endian)
    server_address.sin_port = htons(SERVER_PORT);
    
    // Connect to the server
    // connect() attempts to establish a TCP connection with the server
    // Parameters:
    //   - client_socket: Socket file descriptor
    //   - (struct sockaddr*)&server_address: Server address (cast to generic sockaddr pointer)
    //   - sizeof(server_address): Size of the address structure
    // Returns: 0 on success, -1 on error
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cerr << "Error: Failed to connect to server at " << SERVER_IP << ":" << SERVER_PORT << endl;
        close(client_socket);
        return 1;
    }
    
    cout << "Connected to server at " << SERVER_IP << ":" << SERVER_PORT << endl;
    
    // Send message to server
    const char* message = "Hello from client!";
    
    // send() transmits data through the socket to the connected server
    // Parameters:
    //   - client_socket: Socket file descriptor
    //   - message: Data to send
    //   - strlen(message): Number of bytes to send
    //   - 0: Flags (usually 0 for standard behavior)
    // Returns: Number of bytes sent, or -1 on error
    ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);
    
    if (bytes_sent < 0) {
        cerr << "Error: Failed to send message to server" << endl;
        close(client_socket);
        return 1;
    }
    
    cout << "Message sent to server: " << message << endl;
    cout << "Bytes sent: " << bytes_sent << endl;
    
    // Receive response from server
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    
    // recv() receives data from the connected server through the socket
    // Parameters:
    //   - client_socket: Socket file descriptor
    //   - buffer: Buffer to store received data
    //   - BUFFER_SIZE - 1: Maximum bytes to read (leave space for null terminator)
    //   - 0: Flags (usually 0 for standard behavior)
    // Returns: Number of bytes received, 0 if connection closed, -1 on error
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Null-terminate the received data
        cout << "Response from server: " << buffer << endl;
        cout << "Bytes received: " << bytes_received << endl;
    } else if (bytes_received == 0) {
        cout << "Server closed the connection" << endl;
    } else {
        cerr << "Error: Failed to receive data from server" << endl;
    }
    
    // Close the socket
    // close() terminates the connection and releases socket resources
    close(client_socket);
    
    cout << "Connection closed" << endl;
    
    return 0;
}
