#pragma once

#include <mpi.h>
#include <utility>

#include "generation.hpp"

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
    int num_rows = rows_range.second - rows_range.first + 1;
    int num_cols = cols_range.second - cols_range.first + 1;
    MPI_Send(curr_gen.cells().data(),
             num_rows * num_cols * sizeof(Generation::Cell),
             MPI_BYTE, thread_rank, thread_rank, MPI_COMM_WORLD);
}
void receiveGenPartFromMain(int thread_rank, Generation& t_curr_gen) {
    MPI_Recv(t_curr_gen.cells().data(),
             t_curr_gen.height * t_curr_gen.width * sizeof(Generation::Cell),
             MPI_BYTE, 0, thread_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

} // gol
