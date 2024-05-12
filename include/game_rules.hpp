#pragma once

namespace gol {

inline bool underpopulation(int alive_neighbor_count) {
    return alive_neighbor_count < 2;
}
inline bool nextGeneration(int alive_neighbor_count) {
    return alive_neighbor_count == 2 || alive_neighbor_count == 3;
}
inline bool overpopulation(int alive_neighbor_count) {
    return alive_neighbor_count > 3;
}
inline bool reproduction(int alive_neighbor_count) {
    return alive_neighbor_count == 3;
}

} // gol
