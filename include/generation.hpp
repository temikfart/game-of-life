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

    explicit Generation(int width, int height);

    const Cell& cell(int x, int y) const;
    const std::vector<std::vector<Cell>>& cells() const;
    void setState(int x, int y, bool alive);
    void setRandomStates();

private:
    std::vector<std::vector<Cell>> gen_;
};

} // gol
