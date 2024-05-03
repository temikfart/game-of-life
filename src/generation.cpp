#include "generation.hpp"

namespace gol {

using Cell = Generation::Cell;

Generation::Generation(int width, int height)
    : width(width), height(height), gen_(height, std::vector<Cell>(width)) {}

const Cell& Generation::cell(int x, int y) const {
    return gen_[y][x];
}
const std::vector<std::vector<Cell>>& Generation::cells() const {
    return gen_;
}
void Generation::setState(int x, int y, bool alive) {
    gen_[y][x].alive = alive;
}
void Generation::setRandomStates() {
    for (auto& line : gen_) {
        for (auto& cell : line) {
            cell.alive = (utils::randInt() % kFrequency) == 0;
        }
    }
}

} // gol
