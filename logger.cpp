#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <cstring>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    int PORT = 8080;
    char buffer[1024]; // Buffer to store incoming commands

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return 1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd < 0) continue;

        // Log Connection
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "\n[CONNECT] " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Log Commands/Data
        while (true) {
            memset(buffer, 0, sizeof(buffer)); // Clear buffer
            int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes_read <= 0) {
                std::cout << "[DISCONNECT] Client closed connection." << std::endl;
                break; 
            }

            // Log the received data
            std::cout << "  > Command: " << buffer << std::flush;
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
