#pragma once

#include <iostream>
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
    class Generation {
    public:
        struct Cell {
            bool alive = false;
        };

        int width;
        int height;

        explicit Generation(int width, int height);

        const Cell& cell(int x, int y) const;
        void setState(int x, int y, bool alive);
        void setRandomStates();

    private:
        std::vector<std::vector<Cell>> gen_;
    };

    GameOfLife();

    void run(int gen_count);

private:
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
