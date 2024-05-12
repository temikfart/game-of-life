#pragma once

#include <vector>

#include "utils.hpp"

namespace gol {

constexpr int kFrequency = 3;

class Generation {
public:
    struct Cell {
        bool alive = false;
    };

    int width;
    int height;

    explicit Generation(int width, int height)
        : width(width), height(height), gen_(height, std::vector<Cell>(width)) {}

    const Cell& cell(int x, int y) const {
        return gen_[y][x];
    }
    const std::vector<std::vector<Cell>>& cells() const {
        return gen_;
    }
    void setState(int x, int y, bool alive) {
        gen_[y][x].alive = alive;
    }
    void setRandomStates() {
        for (auto& line : gen_) {
            for (auto& cell : line) {
                cell.alive = (utils::randInt() % kFrequency) == 0;
            }
        }
    }

private:
    std::vector<std::vector<Cell>> gen_;
};

} // gol
