#include "utils.hpp"

namespace gol::utils {

int randInt() {
    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, std::numeric_limits<int>::max());
    return distribution(generator);
}

} // gol::utils
