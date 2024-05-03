#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "generation.hpp"

namespace gol {

constexpr std::string_view default_dir_name = "results";

class GenerationSaver {
public:
    static void saveStartState(int state_id, const Generation& gen);
    static void saveFinalState(int state_id, const Generation& gen);

private:
    static void saveGen(const std::string& filename, const Generation& gen);
    static void createResultDirIfNotExists();
    static std::string getStartFilename(int state_id);
    static std::string getFinalFilename(int state_id);
    static std::string generateName(int state_id);
    static std::string getResultDirPrefixPath();
    static std::string serializeRow(const std::vector<Generation::Cell>& row);
};

} // gol
