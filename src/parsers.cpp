#include "parsers.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

#include "igraph.hpp"

void EdgeListParser::parse(const std::string& filename, IGraph& g) const {
    std::ifstream file(filename);

    if (file.fail()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    int u, v;
    while (file >> u >> v) {
        // TODO: Добавить проверку на непустоту g
        g.addEdge(u, v);
    }

    file.close();
}

void EdgeListParser::parseFromEdgeList(
    const std::vector<std::pair<int, int>>& edgeList, IGraph& g) {
    std::cout << "To be defined!..\n";
}
