#include "game_of_life.hpp"

namespace gol {

using Generation = GameOfLife::Generation;
using Cell = GameOfLife::Generation::Cell;

GameOfLife::Generation::Generation(int width, int height)
    : width(width), height(height), gen_(height, std::vector<Cell>(width)) {}

const Cell& Generation::cell(int x, int y) const {
    return gen_[y][x];
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

GameOfLife::GameOfLife()
    : window_(kWidth, kHeight, kCellSize),
      curr_gen_(kWidth, kHeight),
      next_gen_(kWidth, kHeight) {
    curr_gen_.setRandomStates();
}

void GameOfLife::run(int gen_count) {
    int gen_num = 0;
    while (!window_.closed() && gen_num < gen_count) {
        if (gen_num % 50 == 0)
            std::cout << "Generation #" << gen_num << std::endl;
        calcNextGen();
        drawNextGen();
        std::swap(curr_gen_, next_gen_);
        window_.flushWindow();
        gen_num++;
    }
}
void GameOfLife::calcNextGen() {
    for (int y = 0; y < next_gen_.height; y++) {
        for (int x = 0; x < next_gen_.width; x++) {
            next_gen_.setState(x, y, isCellAliveInCurrGen(x, y));
        }
    }
}
bool GameOfLife::isCellAliveInCurrGen(int x, int y) const {
    int alive_neighbors_count = calcAliveNeighborCount(x, y);
    if (curr_gen_.cell(x, y).alive) {
        if (underpopulation(alive_neighbors_count) || overpopulation(alive_neighbors_count)) {
            return false;
        }
        if (nextGeneration(alive_neighbors_count)) {
            return true;
        }
    }
    return reproduction(alive_neighbors_count);
}
int GameOfLife::calcAliveNeighborCount(int x, int y) const {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < kWidth && ny >= 0 && ny < kHeight) {
                if (curr_gen_.cell(nx, ny).alive) {
                    count++;
                }
            }
        }
    }
    return count;
}
void GameOfLife::drawNextGen() {
    for (int y = 0; y < kHeight; y++) {
        for (int x = 0; x < kWidth; x++) {
            if (next_gen_.cell(x, y).alive) {
                window_.paintCell(x, y);
            }
        }
    }
}

} // gol
