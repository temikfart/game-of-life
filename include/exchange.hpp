#pragma once

#include <mpi.h>
#include <utility>

#include "generation.hpp"
#include "printers.hpp"

namespace gol {

constexpr int kMainThreadRank = 0;

void sendRangesToMain(int thread_rank, const std::pair<int, int>& rows_range,
                      const std::pair<int, int>& cols_range) {
    MPI_Send(&rows_range.first, 1, MPI_INT, kMainThreadRank, thread_rank, MPI_COMM_WORLD);
    MPI_Send(&rows_range.second, 1, MPI_INT, kMainThreadRank, thread_rank, MPI_COMM_WORLD);

    MPI_Send(&cols_range.first, 1, MPI_INT, kMainThreadRank, thread_rank, MPI_COMM_WORLD);
    MPI_Send(&cols_range.second, 1, MPI_INT, kMainThreadRank, thread_rank, MPI_COMM_WORLD);
}
void receiveRangesFromThread(int thread_rank, std::pair<int, int>& rows_range,
                             std::pair<int, int>& cols_range) {
    MPI_Recv(&rows_range.first, 1, MPI_INT, thread_rank, thread_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&rows_range.second, 1, MPI_INT, thread_rank, thread_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv(&cols_range.first, 1, MPI_INT, thread_rank, thread_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&cols_range.second, 1, MPI_INT, thread_rank, thread_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void sendGenPartToThread(int thread_rank, const Generation& curr_gen,
                         const std::pair<int, int>& rows_range,
                         const std::pair<int, int>& cols_range) {
//    printRanges(thread_rank, rows_range, cols_range, true);
    int num_rows = rows_range.second - rows_range.first + 1;
    int num_cols = cols_range.second - cols_range.first + 1;
    int count = num_rows * num_cols * static_cast<int>(sizeof(Generation::Cell));

    auto* cells_to_send = new gol::Generation::Cell[num_rows * num_cols];
    for (int row = rows_range.first; row < rows_range.second; ++row) {
        for (int col = cols_range.first; col <= cols_range.second; ++col) {
            int idx = (row - rows_range.first) * (col - cols_range.first);
            cells_to_send[idx].alive = curr_gen.cell(col, row).alive;
        }
    }
    printGenerationPartDebug(thread_rank, cells_to_send, num_rows * num_cols);
    MPI_Send(cells_to_send, count, MPI_BYTE, thread_rank, thread_rank, MPI_COMM_WORLD);
}
void receiveGenPartFromMain(int thread_rank, Generation& t_curr_gen) {
    int count = t_curr_gen.height * t_curr_gen.width * static_cast<int>(sizeof(Generation::Cell));
//    gol::Generation::Cell* data_ptr = t_curr_gen.cells().data()->data();
    auto* cells_to_receive = new gol::Generation::Cell[t_curr_gen.height * t_curr_gen.width];
    MPI_Recv(cells_to_receive, count, MPI_BYTE, 0, thread_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << "[" << thread_rank << "]";
    for (int i = 0; i < t_curr_gen.height * t_curr_gen.width; ++i) {
        std::cout << " " << cells_to_receive[i].alive;
    }
    std::cout << std::endl;
    printGenerationPartDebug(thread_rank, cells_to_receive, t_curr_gen.height * t_curr_gen.width, true);
}

} // gol
