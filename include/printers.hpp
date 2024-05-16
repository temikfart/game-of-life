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
                 const std::pair<int, int>& cols_range, bool debug = false) {
    std::ostringstream oss;
    if (debug) {
        oss << "[Debug] ";
    }
    oss << "Rank #" << rank << ": ";
    oss << "[" << rows_range.first << ", " << rows_range.second << "]";
    oss << "x";
    oss << "[" << cols_range.first << ", " << cols_range.second << "]";
    std::cout << oss.str() << std::endl;
}
void printGeneration(const std::string& descr, const Generation& gen) {
    std::ostringstream oss;
    oss << descr << std::endl;
    oss << gen << std::endl;
    std::cout << oss.str() << std::endl;
}
void printGenerationPart(int rank, const Generation& gen) {
    std::ostringstream oss;
    oss << "Thread " << rank << " generation:" << std::endl;
    oss << gen << std::endl;
    std::cout << oss.str() << std::endl;
}
void printGenerationPartDebug(int rank, const bool* cells_ptr,
                              int count, bool receive = false) {
    std::ostringstream oss;
    oss << "[Debug] ";
    oss << (receive ? "[RCV]" : "[SND]") << " ";
    oss << "Thread " << rank << " generation:" << std::endl;
    for (int i = 0; i < count; ++i) {
        oss << cells_ptr[i] << " ";
    }
    oss << std::endl;
    std::cout << oss.str() << std::endl;
}
void printGenerationNumber(int num) {
    std::ostringstream oss;
    oss << "Generation #" << num;
    std::cout << oss.str() << std::endl;
}

} // gol
