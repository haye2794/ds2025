#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5001
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Gắn socket vào cổng
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE] = {0};
    read(new_socket, buffer, BUFFER_SIZE);

    std::ifstream file(buffer, std::ios::binary);
    if (!file) {
        const char* error_message = "File not found";
        send(new_socket, error_message, strlen(error_message), 0);
        std::cerr << "File not found: " << buffer << "\n";
    } else {
        std::cout << "Sending file: " << buffer << "\n";
        char file_buffer[BUFFER_SIZE];
        while (!file.eof()) {
            file.read(file_buffer, BUFFER_SIZE);
            send(new_socket, file_buffer, file.gcount(), 0);
        }
        std::cout << "File transfer completed.\n";
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
