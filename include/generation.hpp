#pragma once

#include <vector>

#include "utils.hpp"

namespace gol {

constexpr int kFrequency = 3;

class Generation {
public:
    struct Cell {
        bool alive = false;

        constexpr operator bool() const {
            return alive;
        }
    };

    int height;
    int width;

    explicit Generation(int height, int width)
        : height(height), width(width), gen_(height, std::vector<Cell>(width)) {}

    const Cell& cell(int row, int col) const {
        return gen_[row][col];
    }
    std::vector<std::vector<Cell>>& cells() {
        return gen_;
    }
    const std::vector<std::vector<Cell>>& cells() const {
        return gen_;
    }
    void setState(int row, int col, bool alive) {
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
        for (const auto& cell : rows[row]) {
            os << cell.alive;
        }
        if (row != (rows.size() - 1)) {
            os << std::endl;
        }
    }
    return os;
}

bool operator==(const Generation& lhs, const Generation& rhs) {
    return lhs.height == rhs.height && lhs.width == rhs.width && lhs.cells() == rhs.cells();
}

} // gol
