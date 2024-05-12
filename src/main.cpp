#include "main.hpp"

using namespace gol;

constexpr int generations_num = 1'000;
constexpr int kWidth = 300;
constexpr int kHeight = 300;

Window window(kWidth, kHeight, kCellSize);

void calcNextGen(const Generation& curr_gen, Generation& next_gen) {
    for (int y = 0; y < next_gen.height; y++) {
        for (int x = 0; x < next_gen.width; x++) {
            next_gen.setState(x, y, isCellAliveInCurrGen(curr_gen, x, y));
        }
    }
}
bool isCellAliveInCurrGen(const Generation& curr_gen, int x, int y) {
    int alive_neighbors_count = calcAliveNeighborCount(curr_gen, x, y);
    if (curr_gen.cell(x, y).alive) {
        if (underpopulation(alive_neighbors_count)
            || overpopulation(alive_neighbors_count)) {
            return false;
        }
        if (nextGeneration(alive_neighbors_count)) {
            return true;
        }
    }
    return reproduction(alive_neighbors_count);
}
int calcAliveNeighborCount(const Generation& curr_gen, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < kWidth && ny >= 0 && ny < kHeight) {
                if (curr_gen.cell(nx, ny).alive) {
                    count++;
                }
            }
        }
    }
    return count;
}
void drawNextGen(const Generation& next_gen) {
    for (int y = 0; y < kHeight; y++) {
        for (int x = 0; x < kWidth; x++) {
            if (next_gen.cell(x, y).alive) {
                window.paintCell(x, y);
            }
        }
    }
}

void run(int id, int argc, char* argv[]) {
    Generation curr_gen_(kWidth, kHeight);
    Generation next_gen_(kWidth, kHeight);

    curr_gen_.setRandomStates();
    GenerationSaver::saveStartState(id, curr_gen_);

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int gen_num = 0;
    while (!window.closed() && gen_num < generations_num) {
        if (gen_num % 50 == 0)
            std::cout << "Generation #" << gen_num << std::endl;
        calcNextGen(curr_gen_, next_gen_);
        drawNextGen(next_gen_);

        MPI_Barrier(MPI_COMM_WORLD);
        std::swap(curr_gen_, next_gen_);
        window.flushWindow();
        gen_num++;
    }
    GenerationSaver::saveFinalState(id, curr_gen_);

    MPI_Finalize();
}

int main(int argc, char* argv[]) {
    int id = 0;
    run(id, argc, argv);

    return 0;
}
