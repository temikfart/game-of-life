#pragma once

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <mpi.h>
#include <limits>
#include <random>
#include <vector>
#include <utility>
#include <map>

#include "exchange.hpp"
#include "game_rules.hpp"
#include "generation.hpp"
//#include "generation_saver.hpp"
#include "printers.hpp"
#include "utils.hpp"
#include "validation.hpp"
//#include "window.hpp"

namespace gol {

constexpr int generations_num = 100;

//Window window(kWidth, kHeight, kCellSize);

void calcRanges(int rank, std::pair<int, int>& rows_range, std::pair<int, int>& cols_range) {
    rows_range.first = height * (rank / kCols) + b_th;
    rows_range.second = rows_range.first + height - 1;

    cols_range.first = width * (rank % kRows) + b_th;
    cols_range.second = cols_range.first + width - 1;
}

int calcAliveNeighborCount(const Generation& t_curr_gen, int row, int col) {
    auto isNeighborInGrid = [](int nrow, int ncol, int grid_width, int grid_height) {
        return nrow >= 0 && nrow < grid_height && ncol >= 0 && ncol < grid_width;
    };
    int count = 0;
    for (int dcol = -1; dcol <= 1; dcol++) {
        for (int drow = -1; drow <= 1; drow++) {
            if (dcol == 0 && drow == 0)
                continue;

            int ncol = col + dcol;
            int nrow = row + drow;
            if (isNeighborInGrid(nrow, ncol, t_curr_gen.width, t_curr_gen.height)) {
                if (t_curr_gen.cell(nrow, ncol).alive) {
                    count++;
                }
            }
        }
    }
    return count;
}
bool isCellAliveInCurrGen(const Generation& t_curr_gen, int row, int col) {
    int alive_neighbors_count = calcAliveNeighborCount(t_curr_gen, row, col);
    if (t_curr_gen.cell(row, col).alive) {
        if (underpopulation(alive_neighbors_count)
            || overpopulation(alive_neighbors_count)) {
            return false;
        }
        if (nextGeneration(alive_neighbors_count)) {
            return true;
        }
    }
    return reproduction(alive_neighbors_count);
}
void calcNextGen(const Generation& t_curr_gen, Generation& t_next_gen) {
    for (int row = b_th; row < t_next_gen.height - b_th; row++) {
        for (int col = b_th; col < t_next_gen.width - b_th; col++) {
            t_next_gen.setState(row, col, isCellAliveInCurrGen(t_curr_gen, row, col));
        }
    }
}
//void drawNextGen(const Generation& next_gen) {
//    for (int row = 0; row < kHeight; row++) {
//        for (int col = 0; col < kWidth; col++) {
//            if (next_gen.cell(row, col).alive) {
//                window.paintCell(col, row);
//            }
//        }
//    }
//}

} // gol
