#include "window.hpp"

namespace gol {

Window::Window(int gridWidth, int gridHeight) {
    window.create(sf::VideoMode(CELL_SIZE * gridWidth, CELL_SIZE * gridHeight), "Game of life");
}

bool Window::closeWindowEvent() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return true;
        }
    }
    return false;
}
void Window::setCell(int x, int y) {
    sf::RectangleShape cell;
    cell.setSize({CELL_SIZE, CELL_SIZE});
    cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
    cell.setFillColor(sf::Color::Green);
    window.draw(cell);
}
void Window::flushWindow() {
    window.display();
    window.clear();
}
int Window::randInt() {
    static bool called = false;
    if (!called) {
        std::srand(time(nullptr));
        called = true;
    }
    return std::rand();
}

}
