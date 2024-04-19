#pragma once

#include <algorithm>
#include <limits>
#include <random>
#include <vector>

#include "window.hpp"

#include "utils.hpp"

namespace gol {

constexpr int kWidth = 300;
constexpr int kHeight = 300;
constexpr int kFrequency = 3;

class GameOfLife {
public:
    struct Cell {
    public:
        bool alive = false;
    };


    GameOfLife();

    void run();

private:
    Window window_;
    std::vector<std::vector<Cell>> curr_gen_;
    std::vector<std::vector<Cell>> next_gen_;

    void calcNextGen();
    bool isCellAliveInCurrGen(int x, int y);
    int calcAliveNeighborCount(int x, int y);
    inline bool underpopulation(int alive_neighbor_count) {
        return alive_neighbor_count < 2;
    }
    inline bool nextGeneration(int alive_neighbor_count) {
        return alive_neighbor_count == 2 || alive_neighbor_count == 3;
    }
    inline bool overpopulation(int alive_neighbor_count) {
        return alive_neighbor_count > 3;
    }
    inline bool reproduction(int alive_neighbor_count) {
        return alive_neighbor_count == 3;
    }
    void drawNextGen();
};

} // gol
