#pragma once

#include <cstdlib>

#include "SFML/Graphics.hpp"

namespace gol {

constexpr int kCellSize = 3;

class Window {
public:
    Window(int grid_width, int grid_height, int cell_size) {
        int mode_width = cell_size * grid_width;
        int mode_height = cell_size * grid_height;
        window.create(sf::VideoMode(mode_width, mode_height), "Game of life");
    }
    void paintCell(int x, int y) {
        const auto size = static_cast<float>(kCellSize);
        sf::RectangleShape cell({size, size});

        float x_pos = static_cast<float>(x) * size;
        float y_pos = static_cast<float>(y) * size;
        cell.setPosition(x_pos, y_pos);

        cell.setFillColor(sf::Color::Green);
        window.draw(cell);
    }
    void flushWindow() {
        window.display();
        window.clear();
    }
    bool closed() {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return true;
            }
        }
        return false;
    }

private:
    sf::RenderWindow window;
};

} // gol
