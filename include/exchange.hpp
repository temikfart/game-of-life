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
    int count = num_rows * num_cols;
    int b_th = 1;

    auto* cells_to_send = new bool[count];
    std::ostringstream ss;
    for (int row = rows_range.first - b_th; row <= rows_range.second - b_th; ++row) {
        for (int col = cols_range.first - b_th; col <= cols_range.second - b_th; ++col) {
            int idx = (row + b_th - rows_range.first) * num_cols + (col + b_th - cols_range.first);
            cells_to_send[idx] = curr_gen.cell(row, col).alive;
        }
    }
    MPI_Send(cells_to_send, count, MPI_CXX_BOOL, thread_rank, kDefaultTag, MPI_COMM_WORLD);
    delete[] cells_to_send;
}
void receiveGenPartFromMain(Generation& t_curr_gen) {
    int count = t_curr_gen.height * t_curr_gen.width;
    int b_th = 1;

    auto* cells_to_receive = new bool[t_curr_gen.height * t_curr_gen.width];
    MPI_Recv(cells_to_receive, count, MPI_CXX_BOOL, kMainRank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int num_cols = t_curr_gen.width - b_th * 2;
    for (int row = b_th; row < t_curr_gen.height - b_th; ++row) {
        for (int col = b_th; col < t_curr_gen.width - b_th; ++col) {
            int idx = (row - b_th) * num_cols + (col - b_th);
            t_curr_gen.setState(row, col, cells_to_receive[idx]);
        }
    }
    delete[] cells_to_receive;
}

void sendResultGenToMain(const Generation& t_next_gen) {
    int count = t_next_gen.height * t_next_gen.width;
    int b_th = 1;

    auto* cells_to_send = new bool[count];
    int num_cols = t_next_gen.width - b_th * 2;
    for (int row = b_th; row < t_next_gen.height - b_th; ++row) {
        for (int col = b_th; col < t_next_gen.width - b_th; ++col) {
            int idx = (row - b_th) * num_cols + (col - b_th);
            cells_to_send[idx] = t_next_gen.cell(row, col).alive;
        }
    }
    MPI_Send(cells_to_send, count, MPI_CXX_BOOL, kMainRank, kDefaultTag, MPI_COMM_WORLD);
    delete[] cells_to_send;
}
void receiveResultGenFromThread(int thread_rank, Generation& t_next_gen) {
    int count = t_next_gen.height * t_next_gen.width;
    int b_th = 1;

    auto* cells_to_receive = new bool[count];
    MPI_Recv(cells_to_receive, count, MPI_CXX_BOOL, thread_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int num_cols = t_next_gen.width - b_th * 2;
    for (int row = b_th; row < t_next_gen.height - b_th; ++row) {
        for (int col = b_th; col < t_next_gen.width - b_th; ++col) {
            int idx = (row - b_th) * num_cols + (col - b_th);
            t_next_gen.setState(row, col, cells_to_receive[idx]);
        }
    }
    delete[] cells_to_receive;
}

//void sendReceiveBoundaries() {
//    // communcations order [TL, T, TR, L, R, BL, B, BR]
//    // receive order is the reverse
//
//    MPI_Request requests[8];
//    MPI_Request dummy_request;
//
//    for (int i = 0; i < 8; i++) {
//        // if neighbour exists
//        if (neighbours[i] != -1) {
//            // send data to them
//            MPI_Isend(grid + send_offset[i], 1, MPI_BYTE, neighbours[i],
//                      i, MPI_COMM_WORLD, &dummy_request);
//
//            // wait to recieve from them on the correct communcation tag (reverse order)
//            MPI_Irecv(grid + recv_offset[i], 1, MPI_BYTE, neighbours[i],
//                      7-i, MPI_COMM_WORLD, requests + i);
//
//        }
//            // no neighbour to send to (sink boudnary)
//        else {
//            // send a dud message to yourself to mark request as complete
//            MPI_Isend(nullptr, 0, MPI_BYTE, id, i, MPI_COMM_WORLD, requests + i);
//            MPI_Request_free(requests + i);
//
//        }
//    }
//
//    MPI_Waitall(8, requests, MPI_STATUSES_IGNORE);
//}

} // gol
