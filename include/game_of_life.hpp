#pragma once

#include "window.hpp"

#define GRID_HEIGHT (300)
#define GRID_WIDTH (300)

namespace gol {

class GameOfLife {
public:
    GameOfLife(int gridWidth, int gridHeight);

    int app();
    bool closeWindowEvent();
    void randomGen(bool *gen, int freq);
    void drawGen(const bool *gen);
    void calcGen(const bool *prevGen, bool *nextGen);
    bool isCellAlive(int x, int y, const bool *prevGen);
    int wrapValue(int v, int maxValue);
    void swap(bool *&prevGen, bool *&nextGen);

private:
    Window window;
};

} // gol
