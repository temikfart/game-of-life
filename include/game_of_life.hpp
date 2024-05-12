#pragma once

#include <iostream>
#include <algorithm>
#include <limits>
#include <random>
#include <vector>

#include "generation.hpp"
#include "generation_saver.hpp"
#include "utils.hpp"
#include "window.hpp"

namespace gol {

constexpr int kWidth = 300;
constexpr int kHeight = 300;

class GameOfLife {
public:
    GameOfLife();

    void run(int gen_count);

private:
    static int id_;
    Window window_;
    Generation curr_gen_;
    Generation next_gen_;

    void calcNextGen();
    bool isCellAliveInCurrGen(int x, int y) const;
    int calcAliveNeighborCount(int x, int y) const;
    static inline bool underpopulation(int alive_neighbor_count) {
        return alive_neighbor_count < 2;
    }
    static inline bool nextGeneration(int alive_neighbor_count) {
        return alive_neighbor_count == 2 || alive_neighbor_count == 3;
    }
    static inline bool overpopulation(int alive_neighbor_count) {
        return alive_neighbor_count > 3;
    }
    static inline bool reproduction(int alive_neighbor_count) {
        return alive_neighbor_count == 3;
    }
    void drawNextGen();
};

} // gol
