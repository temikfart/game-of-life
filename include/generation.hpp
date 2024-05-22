#pragma once

#include <vector>

#include "utils.hpp"

namespace gol {

constexpr int kFrequency = 3;

class Generation {
public:
    struct Cell {
        bool alive = false;

        constexpr operator bool() const {
            return alive;
        }
    };
    struct Border {
        int width, height;
        Cell top_left, top_right, bottom_left, bottom_right;
        std::vector<Cell> top, bottom, left, right;

        explicit Border(int width, int height)
            : width(width), height(height),
              top_left(false), top_right(false), bottom_left(false), bottom_right(false),
              top(width, Cell(false)), bottom(width, Cell(false)),
              left(height, Cell(false)), right(height, Cell(false)) {}
    };

    int height;
    int width;

    explicit Generation(int height, int width)
        : height(height), width(width), gen_(height, std::vector<Cell>(width)) {}

    const Cell& cell(int row, int col) const {
        return gen_[row][col];
    }
    Cell& cell(int row, int col) {
        return gen_[row][col];
    }
    std::vector<std::vector<Cell>>& cells() {
        return gen_;
    }
    const std::vector<std::vector<Cell>>& cells() const {
        return gen_;
    }
    void setState(int row, int col, bool alive) {
        gen_[row][col].alive = alive;
    }
    void setRandomStates() {
        for (auto& line : gen_) {
            for (auto& cell : line) {
                cell.alive = (utils::randInt() % kFrequency) == 0;
            }
        }
    }
    Border getBorder() const {
        int b_th = 1;
        int b_w = width - b_th * 2;
        int b_h = height - b_th * 2;
        Generation::Border border(b_w, b_h);

        border.top_left = cell(b_th, b_th);
        border.top_right = cell(b_th, width - b_th * 2);
        border.bottom_left = cell(height - b_th * 2, b_th);
        border.bottom_right = cell(height - b_th * 2, width - b_th * 2);

        int b_col = 0;
        for (int col = b_th; col < width - b_th; ++col, ++b_col) {
            border.top[b_col] = cell(b_th, col);
            border.bottom[b_col] = cell(height - b_th - 1, col);
        }
        int b_row = 0;
        for (int row = b_th; row < height - b_th; ++row, ++b_row) {
            border.left[b_row] = cell(row, b_th);
            border.right[b_row] = cell(row, width - b_th - 1);
        }

        return border;
    }

private:
    std::vector<std::vector<Cell>> gen_;
};

std::ostream& operator<<(std::ostream& os, const Generation& gen) {
    const auto& rows = gen.cells();
    for (unsigned row = 0; row < rows.size(); ++row) {
        for (const auto& cell : rows[row]) {
            os << cell.alive;
        }
        if (row != (rows.size() - 1)) {
            os << std::endl;
        }
    }
    return os;
}

bool operator==(const Generation& lhs, const Generation& rhs) {
    return lhs.height == rhs.height && lhs.width == rhs.width && lhs.cells() == rhs.cells();
}

std::ostream& operator<<(std::ostream& os, const Generation::Border& border) {
    os << border.top_left << " ";
    for (const auto& cell : border.top) {
        os << cell;
    }
    os << " " << border.top_right << "\n" << std::endl;

    for (unsigned row = 0; row < border.left.size(); ++row) {
        os << border.left[row];
        os << std::string(border.width + 2, ' ');
        os << border.right[row];
        os << std::endl;
    }
    os << "\n";

    os << border.bottom_left << " ";
    for (const auto& cell : border.bottom) {
        os << cell;
    }
    os << " " << border.bottom_right;

    return os;
}

} // gol
