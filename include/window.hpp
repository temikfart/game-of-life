#pragma once

#include <SFML/Graphics.hpp>

constexpr int CELL_SIZE = 3;

namespace gol {

class Window {
public:
    Window(int gridWidth, int gridHeight);
    bool closeWindowEvent();
    void setCell(int x, int y);
    void flushWindow();
    int randInt();

private:
    sf::RenderWindow window;
};

} // gol
