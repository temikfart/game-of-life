#include "game_of_life.hpp"

namespace gol {

GameOfLife::GameOfLife()
    : window_(kWidth, kHeight, kCellSize),
      curr_gen_(kHeight, std::vector<Cell>(kWidth)),
      next_gen_(kHeight, std::vector<Cell>(kWidth)) {
    for (auto& line : curr_gen_) {
        for (auto& cell : line) {
            cell.alive = (utils::randInt() % kFrequency) == 0;
        }
    }
}

void GameOfLife::run() {
    while (!window_.closed()) {
        calcNextGen();
        drawNextGen();
        std::swap(curr_gen_, next_gen_);
        window_.flushWindow();
    }
}
void GameOfLife::calcNextGen() {
    for (int y = 0; y < kHeight; y++) {
        for (int x = 0; x < kWidth; x++) {
            next_gen_[y][x].alive = isCellAliveInCurrGen(x, y);
        }
    }
}
bool GameOfLife::isCellAliveInCurrGen(int x, int y) {
    Cell& cell = curr_gen_[y][x];
    int alive_neighbors_count = calcAliveNeighborCount(x, y);
    if (cell.alive) {
        if (underpopulation(alive_neighbors_count) || overpopulation(alive_neighbors_count)) {
            return false;
        }
        if (nextGeneration(alive_neighbors_count)) {
            return true;
        }
    }
    return reproduction(alive_neighbors_count);
}
int GameOfLife::calcAliveNeighborCount(int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < kWidth && ny >= 0 && ny < kHeight) {
                if (curr_gen_[ny][nx].alive) {
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
            if (next_gen_[y][x].alive) {
                window_.paintCell(x, y);
            }
        }
    }
}

} // gol
