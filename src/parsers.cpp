#include "parsers.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "igraph.hpp"

void EdgeListParser::parse(const std::string& filename,
                           IUnweightedGraph& g) const {
    // TODO: Добваить третий аргумент format="raw" /
    // / "snap", "metis", который будет управлять
    // чтением первой строки
    std::ifstream file(filename);

    if (file.fail()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    size_t u, v;
    while (file >> u >> v) {
        // TODO: Добавить проверку на непустоту g
        g.addEdge(u, v);
    }

    file.close();
}

void AdjMatrixParser::parse(const std::string& filename,
                            IUnweightedGraph& g) const {
    // TODO: Добваить третий аргумент format="raw" /
    // / "snap", "metis", который будет управлять
    // чтением первой строки
    std::ifstream file(filename);

    if (file.fail()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    size_t order = peekMatrixSize(file);
    if (order == 0) throw std::runtime_error("Empty matrix or invalid format");

    size_t value;
    int readValues = 0;
    for (size_t i = 0; i < order; ++i) {
        for (size_t j = 0; j < order; ++j) {
            if (!(file >> value)) {
                // матрица неквадратная по каким-то причинам
                std::stringstream error_message;
                error_message << "Unexpected EOF error corrupted data at row "
                              << i << ", col " << j << ". ";
                error_message << "(Read " << readValues
                              << " values, however, expected "
                              << std::pow(order, 2) << ".)";
                throw std::runtime_error(error_message.str());
            }
            if (value != 0) g.addEdge(i, j);
            readValues++;
        }
    }

    file.close();
}

size_t AdjMatrixParser::peekMatrixSize(std::ifstream& file) const {
    std::string firstLine;
    if (!std::getline(file, firstLine)) return 0;

    std::stringstream ss(firstLine);
    size_t count = 0;
    int value;
    while (ss >> value) {
        count++;
    }

    // Возвращаем указатель в начало файла, чтобы основной parse()
    // мог прочитать первую строку заново
    file.clear();
    file.seekg(0, std::ios::beg);

    return count;
}

// void EdgeListParser::parseFromEdgeList(
//     const std::vector<std::pair<int, int>>& edgeList, IGraph& g) {
//     std::cout << "To be defined!..\n";
// }
