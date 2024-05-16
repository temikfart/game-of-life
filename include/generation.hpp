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

    const Cell& cell(int col, int row) const {
        return gen_[row][col];
    }
    std::vector<std::vector<Cell>>& cells() {
        return gen_;
    }
    const std::vector<std::vector<Cell>>& cells() const {
        return gen_;
    }
    void setState(int col, int row, bool alive) {
        gen_[row][col].alive = alive;
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

std::ostream& operator<<(std::ostream& os, const Generation& gen) {
    const auto& rows = gen.cells();
    for (unsigned row = 0; row < rows.size(); ++row) {
        for (auto col : rows[row]) {
            os << col.alive;
        }
        if (row != (rows.size() - 1)) {
            os << std::endl;
        }
    }
    return os;
}

} // gol
