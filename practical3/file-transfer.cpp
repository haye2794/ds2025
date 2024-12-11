#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#define MASTER 0

void send_file(const char* filename, int world_size) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    // Đọc toàn bộ file vào bộ nhớ
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(file_size);
    file.read(buffer.data(), file_size);
    file.close();

    // Gửi kích thước file đến tất cả tiến trình
    MPI_Bcast(&file_size, 1, MPI_UNSIGNED_LONG, MASTER, MPI_COMM_WORLD);

    // Chia file thành các đoạn và gửi
    size_t chunk_size = file_size / (world_size - 1);
    size_t remaining = file_size % (world_size - 1);

    for (int i = 1; i < world_size; ++i) {
        size_t start = (i - 1) * chunk_size;
        size_t size = (i == world_size - 1) ? (chunk_size + remaining) : chunk_size;

        MPI_Send(&size, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
        MPI_Send(buffer.data() + start, size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
}

void receive_file(const char* output_file, int rank) {
    // Nhận kích thước file
    size_t file_size;
    MPI_Bcast(&file_size, 1, MPI_UNSIGNED_LONG, MASTER, MPI_COMM_WORLD);

    // Nhận kích thước đoạn dữ liệu và dữ liệu
    size_t chunk_size;
    MPI_Recv(&chunk_size, 1, MPI_UNSIGNED_LONG, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::vector<char> chunk(chunk_size);
    MPI_Recv(chunk.data(), chunk_size, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Ghi dữ liệu vào file
    std::ofstream file(output_file, std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_file << std::endl;
        return;
    }

    file.write(chunk.data(), chunk_size);
    file.close();
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (argc != 3) {
        if (world_rank == MASTER) {
            std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    if (world_rank == MASTER) {
        send_file(input_file, world_size);
    } else {
        receive_file(output_file, world_rank);
    }

    MPI_Finalize();
    return 0;
}
