#pragma once

#include <algorithm>
#include <iostream>
#include <mpi.h>
#include <limits>
#include <random>
#include <vector>
#include <utility>

#include "exchange.hpp"
#include "game_rules.hpp"
#include "generation.hpp"
#include "generation_saver.hpp"
#include "printers.hpp"
#include "utils.hpp"
#include "validation.hpp"
#include "window.hpp"

void calcRanges(std::pair<int, int>& rows_range, std::pair<int, int>& cols_range, int rank);

void calcNextGen(const gol::Generation& curr_gen, gol::Generation& next_gen);
bool isCellAliveInCurrGen(const gol::Generation& curr_gen, int col, int row);
int calcAliveNeighborCount(const gol::Generation& curr_gen, int col, int row);
void drawNextGen(const gol::Generation& next_gen);
void run(/*int id,*/ int argc, char* argv[]);
