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
    static void saveStartState(int state_id, const Generation& gen) {
        createResultDirIfNotExists();
        saveGen(getStartFilename(state_id), gen);
    }
    static void saveFinalState(int state_id, const Generation& gen) {
        createResultDirIfNotExists();
        saveGen(getFinalFilename(state_id), gen);
    }

private:
    static void saveGen(const std::string& filename, const Generation& gen) {
        std::ofstream file(filename);
        file << gen.width << " " << gen.height << std::endl;
        for (const auto& row: gen.cells()) {
            file << serializeRow(row) << std::endl;
        }
        file.close();
    }
    static void createResultDirIfNotExists() {
        if (!std::filesystem::exists("results")) {
            std::filesystem::create_directory("results");
        }
    }
    static std::string getStartFilename(int state_id) {
        return getResultDirPrefixPath() + generateName(state_id) + "_a.txt";
    }
    static std::string getFinalFilename(int state_id) {
        return getResultDirPrefixPath() + generateName(state_id) + "_b.txt";
    }
    static std::string generateName(int state_id) {
        return "gol_run_" + std::to_string(state_id);
    }
    static std::string getResultDirPrefixPath() {
        return std::string(default_dir_name) + "/";
    }
    static std::string serializeRow(const std::vector<Generation::Cell>& row) {
        std::stringstream ss;
        for (const auto& cell: row) {
            ss << cell.alive;
        }
        return ss.str();
    }
};

} // gol
