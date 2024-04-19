#pragma once

#include <cstdlib>

#include <SFML/Graphics.hpp>

namespace gol {

constexpr int kCellSize = 3;

class Window {
public:
    Window(int grid_width, int grid_height, int cell_size);
    void paintCell(int x, int y, sf::Color color = sf::Color::Green);
    void flushWindow();
    bool closed();

private:
    sf::RenderWindow window;
};

} // gol
