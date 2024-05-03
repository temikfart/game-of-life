#include "generation_saver.hpp"

namespace gol {

void GenerationSaver::saveStartState(int state_id, const Generation& gen) {
    createResultDirIfNotExists();
    saveGen(getStartFilename(state_id), gen);
}
void GenerationSaver::saveFinalState(int state_id, const Generation& gen) {
    createResultDirIfNotExists();
    saveGen(getFinalFilename(state_id), gen);
}

void GenerationSaver::saveGen(const std::string& filename, const Generation& gen) {
    std::ofstream file(filename);
    file << gen.width << " " << gen.height << std::endl;
    for (const auto& row: gen.cells()) {
        file << serializeRow(row) << std::endl;
    }
    file.close();
}
void GenerationSaver::createResultDirIfNotExists() {
    if (!std::filesystem::exists("results")) {
        std::filesystem::create_directory("results");
    }
}
std::string GenerationSaver::getStartFilename(int state_id) {
    return getResultDirPrefixPath() + generateName(state_id) + "_a.txt";
}
std::string GenerationSaver::getFinalFilename(int state_id) {
    return getResultDirPrefixPath() + generateName(state_id) + "_b.txt";
}
std::string GenerationSaver::generateName(int state_id) {
    return "gol_run_" + std::to_string(state_id);
}
std::string GenerationSaver::getResultDirPrefixPath() {
    return std::string(default_dir_name) + "/";
}
std::string GenerationSaver::serializeRow(const std::vector<Generation::Cell>& row) {
    std::stringstream ss;
    for (const auto& cell: row) {
        ss << cell.alive;
    }
    return ss.str();
}

} // gol
