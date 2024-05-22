#include "main.hpp"

using namespace gol;

using Range = std::pair<int, int>;

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
//        printGeneration("Start generation:", curr_gen_);
        std::cout << std::endl;
    }

    /** Calculate ranges for the thread */
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // +2 to include the borders
    int t_gen_height = height + 2;
    int t_gen_width = width + 2;
    Generation t_curr_gen(t_gen_height, t_gen_width), t_next_gen(t_gen_height, t_gen_width);
    Range t_rows_range, t_cols_range;
    calcRanges(rank, t_rows_range, t_cols_range);

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

    /** Calculate next generation `generation_num` times */
    MPI_Barrier(MPI_COMM_WORLD);
    int gen_num = 0;
    while (/*!window.closed() &&*/ gen_num < generations_num) {
//        if (rank == 0 && gen_num % 50 == 0)
//            printGenerationNumber(gen_num);

        sendReceiveBoundaries(rank, t_curr_gen);
        calcNextGen(t_curr_gen, t_next_gen);
//        drawNextGen(t_next_gen);

        std::swap(t_curr_gen, t_next_gen);
//        window.flushWindow();
        gen_num++;
    }

    /** Collect final Generation */
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        for (int row = t_rows_range.first - 1; row <= t_rows_range.second - 1; ++row) {
            for (int col = t_cols_range.first - 1; col <= t_cols_range.second - 1; ++col) {
                int t_row = row + 2 - t_rows_range.first;
                int t_col = col + 2 - t_cols_range.first;
                final_gen_.setState(row, col, t_curr_gen.cell(t_row, t_col));
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
        sendResultGenToMain(t_curr_gen);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    if (rank == 0) {
        double total_time = end_time - start_time;
        std::cout << std::setw(20) << std::left << "Total execution time: "
                  << std::setprecision(6) << std::fixed << std::setw(10) << std::right
                  << total_time << std::endl;
    }

//    if (rank == 0) {
//        printGeneration("Final generation:", final_gen_);
//    }

    MPI_Finalize();
//    GenerationSaver::saveFinalState(id, final_gen_);
}

int main(int argc, char* argv[]) {
    run(argc, argv);

    return 0;
}
