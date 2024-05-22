#include "main.hpp"

using namespace gol;

using Range = std::pair<int, int>;

//constexpr int generations_num = 1'000;

Window window(kWidth, kHeight, kCellSize);

void calcRanges(int rank, Range& rows_range, Range& cols_range) {
    const int b_th = 1;

    rows_range.first = height * (rank / kRows) + b_th;
    rows_range.second = rows_range.first + height - 1;

    cols_range.first = width * (rank % kCols) + b_th;
    cols_range.second = cols_range.first + width - 1;
}

void calcNextGen(const Generation& curr_gen, Generation& next_gen) {
    for (int row = 0; row < next_gen.height; row++) {
        for (int col = 0; col < next_gen.width; col++) {
            next_gen.setState(row, col, isCellAliveInCurrGen(curr_gen, row, col));
        }
    }
}
bool isCellAliveInCurrGen(const Generation& curr_gen, int row, int col) {
    int alive_neighbors_count = calcAliveNeighborCount(curr_gen, row, col);
    if (curr_gen.cell(row, col).alive) {
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
bool isNeighborInGrid(int nrow, int ncol, int grid_width, int grid_height) {
    return nrow >= 0 && nrow < grid_height && ncol >= 0 && ncol < grid_width;
}
int calcAliveNeighborCount(const Generation& curr_gen, int row, int col) {
    int count = 0;
    for (int dcol = -1; dcol <= 1; dcol++) {
        for (int drow = -1; drow <= 1; drow++) {
            if (dcol == 0 && drow == 0)
                continue;

            int ncol = col + dcol;
            int nrow = row + drow;
            if (isNeighborInGrid(nrow, ncol, curr_gen.width, curr_gen.height)) {
                if (curr_gen.cell(nrow, ncol).alive) {
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
            if (next_gen.cell(row, col).alive) {
                window.paintCell(col, row);
            }
        }
    }
}

void run(/*int id,*/ int argc, char* argv[]) {
    Generation curr_gen_(kHeight, kWidth), final_gen_(kHeight, kWidth);
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
        printGeneration("Start generation:", curr_gen_);
        std::cout << std::endl;
    }

    /** Calculate ranges for the thread */
    MPI_Barrier(MPI_COMM_WORLD);
    // +2 to include the borders
    int t_gen_height = height + 2;
    int t_gen_width = width + 2;
    Generation t_curr_gen(t_gen_height, t_gen_width), t_next_gen(t_gen_height, t_gen_width);
    Range t_rows_range, t_cols_range;
    calcRanges(rank, t_rows_range, t_cols_range);
//    printRanges(rank, t_rows_range, t_cols_range, true);

    /** Set start states for all threads */
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        for (int row = t_rows_range.first - 1; row <= t_rows_range.second - 1; ++row) {
            for (int col = t_cols_range.first - 1; col <= t_cols_range.second - 1; ++col) {
                t_curr_gen.setState(row + 2 - t_rows_range.first,
                                    col + 2 - t_cols_range.first,
                                    curr_gen_.cell(row, col).alive);
            }
        }
    }

    std::vector<Range> rows_ranges, cols_ranges;
    if (rank == 0) {
        rows_ranges.resize(size - 1);
        cols_ranges.resize(size - 1);
        for (int t = 1; t < size; ++t) {
            receiveRangesFromThread(t, rows_ranges[t - 1], cols_ranges[t - 1]);
            sendGenPartToThread(t, curr_gen_, rows_ranges[t - 1], cols_ranges[t - 1]);
        }
    } else {
        sendRangesToMain(t_rows_range, t_cols_range);
        receiveGenPartFromMain(t_curr_gen);
    }
    printGenerationPart(rank, t_curr_gen, true);

    /** Calculate next generation `generation_num` times */
    MPI_Barrier(MPI_COMM_WORLD);
//    int gen_num = 0;
//    while (!window.closed() && gen_num < generations_num) {
//        if (rank == 0 && gen_num % 50 == 0)
//            printGenerationNumber(gen_num);
//
//        calcNextGen(t_curr_gen, t_next_gen);
//        drawNextGen(t_next_gen);
//
//        MPI_Barrier(MPI_COMM_WORLD);
        std::swap(t_curr_gen, t_next_gen);
//        window.flushWindow();
//        gen_num++;
//    }

    /** Collect final Generation */
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        for (int row = t_rows_range.first - 1; row <= t_rows_range.second - 1; ++row) {
            for (int col = t_cols_range.first - 1; col <= t_cols_range.second - 1; ++col) {
                int t_row = row + 2 - t_rows_range.first;
                int t_col = col + 2 - t_cols_range.first;
                final_gen_.setState(row, col, t_next_gen.cell(t_row, t_col));
            }
        }

        for (int t = 1; t < size; ++t) {
            Generation result_gen(t_gen_height, t_gen_width);
            receiveResultGenFromThread(t, result_gen);

            auto& rows_range = rows_ranges[t - 1];
            auto& cols_range = cols_ranges[t - 1];
            for (int row = rows_range.first - 1; row <= rows_range.second - 1; ++row) {
                for (int col = cols_range.first - 1; col <= cols_range.second - 1; ++col) {
                    int t_row = row + 2 - rows_range.first;
                    int t_col = col + 2 - cols_range.first;
                    final_gen_.setState(row, col, result_gen.cell(t_row, t_col).alive);
                }
            }
        }
    } else {
        sendResultGenToMain(t_next_gen);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printGeneration("Final generation:", final_gen_);
        std::cout << "Current and Final generations are equal: "
                  << (final_gen_ == curr_gen_) << std::endl;
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
