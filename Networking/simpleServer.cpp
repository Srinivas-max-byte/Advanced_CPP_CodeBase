#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

using namespace std;

void handle_client(int client_socket, struct sockaddr_in client_address) {
    const int BUFFER_SIZE = 1024;
    
    // Get client IP address and port
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_address.sin_port);
    
    cout << "\nClient connected from " << client_ip << ":" << client_port << endl;
    
    // Receive data from client
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        cout << "Received from client: " << buffer << endl;
        
        // Send response back to client
        const char* response = "Message received by server!";
        send(client_socket, response, strlen(response), 0);
        cout << "Response sent to client" << endl;
    } else if (bytes_received == 0) {
        cout << "Client disconnected" << endl;
    } else {
        cerr << "Error: Failed to receive data from client" << endl;
    }
    
    // Close the client socket
    cout << "Closing client socket" << endl;
    close(client_socket);

    cout << "Waiting for 2 minutes before closing the client connection handler" << endl;
    sleep(120);
}

int main() {
    const int PORT = 8080;
    const int BACKLOG = 5;
    
    // Create a socket
    // socket() parameters:
    //   - AF_INET: IPv4 address family
    //   - SOCK_STREAM: TCP protocol (connection-oriented, reliable, ordered delivery)
    //   - 0: Let the system choose the appropriate protocol (IPPROTO_TCP for SOCK_STREAM)
    // Returns a file descriptor (socket handle) if successful, or -1 on error
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_socket < 0) {
        cerr << "Error: Failed to create socket" << endl;
        return 1;
    }
    
    cout << "Socket created successfully" << endl;
    
    // Define server address structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    
    // sin_family specifies the address family (IPv4 in this case using AF_INET)
    server_address.sin_family = AF_INET;
    // sin_addr.s_addr is set to INADDR_ANY which means the server will accept connections on any available IPv4 interface (0.0.0.0)
    // htonl() "Host to Network Long" converts the address from host byte order to network byte order (big-endian)
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    // sin_port specifies the port number where the server will listen; htons() "Host to Network Short" converts from host to network byte order (big-endian)
    server_address.sin_port = htons(PORT);
    
    // Bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cerr << "Error: Failed to bind socket to port " << PORT << endl;
        close(server_socket);
        return 1;
    }
    
    cout << "Socket bound to port " << PORT << endl;
    
    // Listen for incoming connections
    // BACKLOG: Maximum number of queued connections
    if (listen(server_socket, BACKLOG) < 0) {
        cerr << "Error: Failed to listen on socket" << endl;
        close(server_socket);
        return 1;
    }
    
    cout << "Server listening on port " << PORT << "..." << endl;
    
    // Accept incoming connections and communicate
    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        
        // Accept a connection from a client
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        
        if (client_socket < 0) {
            cerr << "Error: Failed to accept connection" << endl;
            continue;
        }
        
        // Create a detached thread to handle the client
        thread client_thread(handle_client, client_socket, client_address);
        client_thread.detach();
    }
    
    // Close the server socket (this line is never reached in infinite loop)
    close(server_socket);
    
    return 0;
}
