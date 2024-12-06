#include <iostream>
#include <rpc/rpc.h>
#include <fstream>
#include <cstring>

#define FILE_TRANSFER_PROG 0x31234567
#define FILE_TRANSFER_VERS 1
#define GET_FILE_PROC 1

// Hàm RPC xử lý yêu cầu lấy nội dung file
char* get_file(char* file_name) {
    static char file_content[1024]; // Dùng static để giữ dữ liệu giữa các lệnh gọi
    std::ifstream file(file_name, std::ios::binary);

    if (!file) {
        snprintf(file_content, sizeof(file_content), "Error: File '%s' not found", file_name);
    } else {
        file.read(file_content, sizeof(file_content) - 1);
        file_content[file.gcount()] = '\0'; // Kết thúc nội dung
    }

    return file_content;
}

int main() {
    // Đăng ký hàm RPC
    registerrpc(
        FILE_TRANSFER_PROG, FILE_TRANSFER_VERS, GET_FILE_PROC,
        (char*(*)(char*))get_file, // Hàm xử lý
        (xdrproc_t)xdr_wrapstring, (xdrproc_t)xdr_wrapstring // Định dạng dữ liệu
    );

    std::cout << "RPC Server is running..." << std::endl;

    // Chạy server
    svc_run();

    return 0;
}
