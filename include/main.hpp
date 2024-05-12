#pragma once

#include <algorithm>
#include <iostream>
#include <mpi.h>
#include <limits>
#include <random>
#include <vector>

#include "game_rules.hpp"
#include "generation.hpp"
#include "generation_saver.hpp"
#include "utils.hpp"
#include "window.hpp"

void calcNextGen(const gol::Generation& curr_gen, gol::Generation& next_gen);
bool isCellAliveInCurrGen(const gol::Generation& curr_gen, int x, int y);
int calcAliveNeighborCount(const gol::Generation& curr_gen, int x, int y);
void drawNextGen(const gol::Generation& next_gen);
void run(int id, int argc, char* argv[]);
