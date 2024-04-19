#include "window.hpp"

namespace gol {

Window::Window(int grid_width, int grid_height, int cell_size) {
    int mode_width = cell_size * grid_width;
    int mode_height = cell_size * grid_height;
    window.create(sf::VideoMode(mode_width, mode_height), "Game of life");
}

void Window::paintCell(int x, int y, sf::Color color) {
    const auto size = static_cast<float>(kCellSize);
    sf::RectangleShape cell({size, size});

    float x_pos = static_cast<float>(x) * size;
    float y_pos = static_cast<float>(y) * size;
    cell.setPosition(x_pos, y_pos);

    cell.setFillColor(color);
    window.draw(cell);
}
void Window::flushWindow() {
    window.display();
    window.clear();
}
bool Window::closed() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return true;
        }
    }
    return false;
}

}
