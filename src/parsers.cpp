#include "parsers.hpp"

#include <fstream>
#include <stdexcept>

#include "igraph.hpp"

void EdgeListParser::parse(const std::string& filename, IGraph& g) const {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }

    int u, v;
    while (file >> u >> v) {
        // TODO: Добавить проверку на непустоту g
        g.addEdge(u, v);
    }
}
