#include "main.hpp"

using namespace gol;

using Range = std::pair<int, int>;

constexpr int generations_num = 1'000;
constexpr int kWidth = 6;
constexpr int kHeight = 6;
constexpr int kRows = 2;
constexpr int kCols = 2;
constexpr int width = kWidth / kCols;
constexpr int height = kHeight / kRows;

Window window(kWidth, kHeight, kCellSize);

void calcRanges(Range& rows_range, Range& cols_range, int rank) {
    rows_range.first = height * (rank / kRows);
    rows_range.second = rows_range.first + height - 1;

    cols_range.first = width * (rank % kCols);
    cols_range.second = cols_range.first + width - 1;
}

void calcNextGen(const Generation& curr_gen, Generation& next_gen) {
    for (int row = 0; row < next_gen.height; row++) {
        for (int col = 0; col < next_gen.width; col++) {
            next_gen.setState(col, row, isCellAliveInCurrGen(curr_gen, col, row));
        }
    }
}
bool isCellAliveInCurrGen(const Generation& curr_gen, int col, int row) {
    int alive_neighbors_count = calcAliveNeighborCount(curr_gen, col, row);
    if (curr_gen.cell(col, row).alive) {
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
int calcAliveNeighborCount(const Generation& curr_gen, int col, int row) {
    int count = 0;
    for (int dcol = -1; dcol <= 1; dcol++) {
        for (int drow = -1; drow <= 1; drow++) {
            if (dcol == 0 && drow == 0)
                continue;

            int ncol = col + dcol;
            int nrow = row + drow;
            if (ncol >= 0 && ncol < kWidth && nrow >= 0 && nrow < kHeight) {
                if (curr_gen.cell(ncol, nrow).alive) {
                    count++;
                }
            }
        }
    }
    return count;
}
void drawNextGen(const Generation& next_gen) {
    for (int row = 0; row < kHeight; row++) {
        for (int col = 0; col < kWidth; col++) {
            if (next_gen.cell(col, row).alive) {
                window.paintCell(col, row);
            }
        }
    }
}

void run(/*int id,*/ int argc, char* argv[]) {
    Generation curr_gen_(kWidth, kHeight), final_gen_(kWidth, kHeight);
    curr_gen_.setRandomStates();
//    GenerationSaver::saveStartState(id, curr_gen_);

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) {
        validateNumberOfProcesses(size, kRows * kCols);
    }

    if (rank == 0) {
        printGridSize(kHeight, kWidth);
        printGeneration(curr_gen_);
        std::cout << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    Generation t_curr_gen(width, height), t_next_gen(width, height);
    Range t_rows_range, t_cols_range;
    calcRanges(t_rows_range, t_cols_range, rank);
//    printRanges(rank, t_rows_range, t_cols_range);
    MPI_Barrier(MPI_COMM_WORLD);

    // Get start states
    if (rank == 0) {
        for (int col = t_cols_range.first; col < t_cols_range.second; ++col) {
            for (int row = t_rows_range.first; row < t_rows_range.second; ++row) {
                t_curr_gen.setState(col, row, curr_gen_.cell(col, row).alive);
            }
        }
        printGenerationPart(rank, t_curr_gen);

        for (int t = 1; t < size; ++t) {
            Range rows_range, cols_range;
            receiveRangesFromThread(t, rows_range, cols_range);
//            printRanges(t, rows_range, cols_range);
            sendGenPartToThread(t, curr_gen_, rows_range, cols_range);
        }
    } else {
        sendRangesToMain(rank, t_rows_range, t_cols_range);
        receiveGenPartFromMain(rank, t_curr_gen);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printGenerationPart(rank, t_curr_gen);

    int gen_num = 0;
    while (!window.closed() && gen_num < generations_num) {
        if (rank == 0 && gen_num % 50 == 0)
            printGenerationNumber(gen_num);

        calcNextGen(t_curr_gen, t_next_gen);
        drawNextGen(t_next_gen);

        MPI_Barrier(MPI_COMM_WORLD);
        std::swap(t_curr_gen, t_next_gen);
        window.flushWindow();
        gen_num++;
    }

    MPI_Finalize();
//    GenerationSaver::saveFinalState(id, final_gen_);
}

int main(int argc, char* argv[]) {
//    int id = 0;
//    run(id, argc, argv);
    run(argc, argv);

    return 0;
}
