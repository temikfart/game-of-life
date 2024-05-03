#include "main.hpp"

constexpr int generations_num = 1'000;

int main() {
    gol::GameOfLife().run(generations_num);

    return 0;
}
