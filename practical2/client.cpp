#include <iostream>
#include <rpc/rpc.h>
#include <fstream>

#define FILE_TRANSFER_PROG 0x31234567
#define FILE_TRANSFER_VERS 1
#define GET_FILE_PROC 1

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_host> <file_name>" << std::endl;
        return 1;
    }

    char* server_host = argv[1];
    char* file_name = argv[2];

    // Tạo client RPC
    CLIENT* client = clnt_create(server_host, FILE_TRANSFER_PROG, FILE_TRANSFER_VERS, "udp");
    if (!client) {
        clnt_pcreateerror(server_host);
        return 1;
    }

    // Gọi hàm RPC
    char** result = (char**)clnt_call(
        client, GET_FILE_PROC, 
        (xdrproc_t)xdr_wrapstring, (char*)&file_name,
        (xdrproc_t)xdr_wrapstring, (char**)nullptr, 
        {0, 0}
    );

    if (!result) {
        clnt_perror(client, server_host);
    } else {
        std::cout << "File content:\n" << *result << std::endl;

        // Lưu nội dung file ra file cục bộ
        std::ofstream output("received_file.txt");
        if (output) {
            output << *result;
            std::cout << "File saved to 'received_file.txt'" << std::endl;
        }
    }

    clnt_destroy(client);
    return 0;
}
