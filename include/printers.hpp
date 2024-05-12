#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace gol {

void printGridSize(int height, int width) {
    std::ostringstream oss;
    oss <<  "Grid size: " << height << "x" << width;
    std::cout << oss.str() << std::endl;
}
void printRanges(int rank, const std::pair<int, int>& rows_range,
                 const std::pair<int, int>& cols_range) {
    std::ostringstream oss;
    oss << "Rank #" << rank << ": ";
    oss << "[" << rows_range.first << ", " << rows_range.second << "]";
    oss << "x";
    oss << "[" << cols_range.first << ", " << cols_range.second << "]";
    std::cout << oss.str() << std::endl;
}
void printGenerationNumber(int num) {
    std::ostringstream oss;
    oss << "Generation #" << num;
    std::cout << oss.str() << std::endl;
}

} // gol
