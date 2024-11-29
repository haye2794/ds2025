#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5001
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Kết nối tới server
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    std::string filename;
    std::cout << "Enter the filename: ";
    std::cin >> filename;

    send(sock, filename.c_str(), filename.size(), 0);

    std::ofstream output_file("received_" + filename, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error creating file to save data.\n";
        return -1;
    }

    std::cout << "Receiving file...\n";
    int bytes_received;
    while ((bytes_received = read(sock, buffer, BUFFER_SIZE)) > 0) {
        output_file.write(buffer, bytes_received);
    }

    std::cout << "File received and saved as 'received_" << filename << "'\n";

    output_file.close();
    close(sock);
    return 0;
}
