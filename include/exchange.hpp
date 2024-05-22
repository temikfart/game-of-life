#pragma once

#include <mpi.h>
#include <utility>

#include "generation.hpp"
#include "printers.hpp"

namespace gol {

constexpr int b_th = 1;
constexpr int kWidth = 6;
constexpr int kHeight = 6;
constexpr int kRows = 2;
constexpr int kCols = 2;
constexpr int width = kWidth / kCols;
constexpr int height = kHeight / kRows;

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

void sendCornerCell(int dst_rank, const Generation::Cell& cell) {
    MPI_Send(&cell.alive, 1, MPI_CXX_BOOL, dst_rank, kDefaultTag, MPI_COMM_WORLD);
}
void receiveCornerCell(int src_rank, Generation::Cell& cell) {
    MPI_Recv(&cell.alive, 1, MPI_CXX_BOOL, src_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
void sendBoundaryCells(int dst_rank, int count, const bool* cells) {
    MPI_Send(cells, count, MPI_CXX_BOOL, dst_rank, kDefaultTag, MPI_COMM_WORLD);
}
void receiveBoundaryCells(int src_rank, int count, bool* cells) {
    MPI_Recv(cells, count, MPI_CXX_BOOL, src_rank, kDefaultTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
void sendReceiveBoundaries(int rank, Generation& t_curr_gen) {
    bool isTopRow = rank >= 0 && rank < kCols;
    bool isBottomRow = (rank >= (kRows - 1) * kCols) && (rank < kRows * kCols);
    bool isLeftCol = rank % kCols == 0;
    bool isRightCol = rank % kCols == kCols - 1;

    Generation::Border t_border = t_curr_gen.getBorder();

    if (!isTopRow && !isLeftCol) {
        /** Send Top Left cell */
        sendCornerCell(rank - kCols - 1, t_border.top_left);
    }
    if (!isBottomRow && !isRightCol) {
        /** Receive Bottom Right cell */
        receiveCornerCell(rank + kCols + 1,
                          t_curr_gen.cell(t_curr_gen.height - 1, t_curr_gen.width - 1));
    }

    if (!isTopRow && !isRightCol) {
        /** Send Top Right cell */
        sendCornerCell(rank - kCols + 1, t_border.top_right);
    }
    if (!isBottomRow && !isLeftCol) {
        /** Receive Bottom Left cell */
        receiveCornerCell(rank + kCols - 1, t_curr_gen.cell(t_curr_gen.height - 1, 0));
    }

    if (!isBottomRow && !isRightCol) {
        /** Send Bottom Right cell */
        sendCornerCell(rank + kCols + 1, t_border.bottom_right);
    }
    if (!isTopRow &&!isLeftCol) {
        /** Receive Top Left cell */
        receiveCornerCell(rank - kCols - 1, t_curr_gen.cell(0, 0));
    }

    if (!isBottomRow && !isLeftCol) {
        /** Send Bottom Left cell */
        sendCornerCell(rank + kCols - 1, t_border.bottom_left);
    }
    if (!isTopRow && !isRightCol) {
        /** Receive Top Right cell */
        receiveCornerCell(rank - kCols + 1, t_curr_gen.cell(0, t_curr_gen.width - 1));
    }

    int horizontal_count = t_border.width;
    if (!isTopRow) {
        /** Send Top cells */
        auto* top_row = new bool[horizontal_count];
        for (int col = 0; col < horizontal_count; ++col) {
            top_row[col] = t_border.top[col].alive;
        }
        sendBoundaryCells(rank - kCols, horizontal_count, top_row);
        delete[] top_row;
    }
    if (!isBottomRow) {
        /** Receive Bottom cells */
        auto* bottom_row = new bool[horizontal_count];
        receiveBoundaryCells(rank + kCols, horizontal_count, bottom_row);
        for (int col = 0; col < horizontal_count; ++col) {
            t_curr_gen.cell(t_curr_gen.height - 1, 1 + col).alive = bottom_row[col];
        }
        delete[] bottom_row;
    }

    if (!isBottomRow) {
        /** Send Bottom cells */
        auto* bottom_row = new bool[horizontal_count];
        for (int col = 0; col < horizontal_count; ++col) {
            bottom_row[col] = t_border.bottom[col].alive;
        }
        sendBoundaryCells(rank + kCols, horizontal_count, bottom_row);
        delete[] bottom_row;
    }
    if (!isTopRow) {
        /** Receive Top cells */
        auto* top_row = new bool[horizontal_count];
        receiveBoundaryCells(rank - kCols, horizontal_count, top_row);
        for (int col = 0; col < horizontal_count; ++col) {
            t_curr_gen.cell(0, 1 + col).alive = top_row[col];
        }
        delete[] top_row;
    }

    int vertical_count = t_border.height;
    if (!isLeftCol) {
        /** Send Left cells */
        auto* left_col = new bool[vertical_count];
        for (int row = 0; row < vertical_count; ++row) {
            left_col[row] = t_border.left[row].alive;
        }
        sendBoundaryCells(rank - 1, vertical_count, left_col);
        delete[] left_col;
    }
    if (!isRightCol) {
        /** Receive Right cells */
        auto* right_col = new bool[vertical_count];
        receiveBoundaryCells(rank + 1, vertical_count, right_col);
        for (int row = 0; row < vertical_count; ++row) {
            t_curr_gen.cell(1 + row, t_curr_gen.width - 1).alive = right_col[row];
        }
        delete[] right_col;
    }

    if (!isRightCol) {
        /** Send Right cells */
        auto* right_col = new bool[vertical_count];
        for (int row = 0; row < vertical_count; ++row) {
            right_col[row] = t_border.right[row].alive;
        }
        sendBoundaryCells(rank + 1, vertical_count, right_col);
        delete[] right_col;
    }
    if (!isLeftCol) {
        /** Receive Left cells */
        auto* left_col = new bool[vertical_count];
        receiveBoundaryCells(rank - 1, vertical_count, left_col);
        for (int row = 0; row < vertical_count; ++row) {
            t_curr_gen.cell(1 + row, 0).alive = left_col[row];
        }
        delete[] left_col;
    }
}

} // gol
