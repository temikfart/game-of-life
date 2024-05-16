#pragma once

#include <mpi.h>
#include <utility>

#include "generation.hpp"
#include "printers.hpp"

namespace gol {

constexpr int kMainRank = 0;
constexpr int kDefaultTag = 0;

void sendRangesToMain(const std::pair<int, int>& rows_range,
                      const std::pair<int, int>& cols_range) {
    MPI_Send(&rows_range.first, 1, MPI_INT, kMainRank, kDefaultTag, MPI_COMM_WORLD);
    MPI_Send(&rows_range.second, 1, MPI_INT, kMainRank, kDefaultTag, MPI_COMM_WORLD);

    MPI_Send(&cols_range.first, 1, MPI_INT, kMainRank, kDefaultTag, MPI_COMM_WORLD);
    MPI_Send(&cols_range.second, 1, MPI_INT, kMainRank, kDefaultTag, MPI_COMM_WORLD);
}
void receiveRangesFromThread(int thread_rank, std::pair<int, int>& rows_range,
                             std::pair<int, int>& cols_range) {
    MPI_Recv(&rows_range.first, 1, MPI_INT, thread_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&rows_range.second, 1, MPI_INT, thread_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Recv(&cols_range.first, 1, MPI_INT, thread_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&cols_range.second, 1, MPI_INT, thread_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void sendGenPartToThread(int thread_rank, const Generation& curr_gen,
                         const std::pair<int, int>& rows_range,
                         const std::pair<int, int>& cols_range) {
    int num_rows = rows_range.second - rows_range.first + 1;
    int num_cols = cols_range.second - cols_range.first + 1;
    int row_width = cols_range.second - cols_range.first + 1;
    int count = num_rows * num_cols;

    auto* cells_to_send = new bool[count];
    for (int row = rows_range.first; row <= rows_range.second; ++row) {
        for (int col = cols_range.first; col <= cols_range.second; ++col) {
            int idx = (row - rows_range.first) * row_width + (col - cols_range.first);
            cells_to_send[idx] = curr_gen.cell(row, col).alive;
        }
    }
    MPI_Send(cells_to_send, count, MPI_CXX_BOOL, thread_rank, kDefaultTag, MPI_COMM_WORLD);
    delete[] cells_to_send;
}
void receiveGenPartFromMain(Generation& t_curr_gen) {
    int count = t_curr_gen.height * t_curr_gen.width;

    auto* cells_to_receive = new bool[t_curr_gen.height * t_curr_gen.width];
    MPI_Recv(cells_to_receive, count, MPI_CXX_BOOL, kMainRank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int row = 0; row < t_curr_gen.height; ++row) {
        for (int col = 0; col < t_curr_gen.width; ++col) {
            int idx = (row * t_curr_gen.width) + col;
            t_curr_gen.setState(row, col, cells_to_receive[idx]);
        }
    }
    delete[] cells_to_receive;
}

} // gol
