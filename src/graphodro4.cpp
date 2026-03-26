#include <iostream>

#include "graphs.hpp"
#include "parsers.hpp"

using Graph = AdjacencyList;
using Parser = SNAPParser;

int main() {
    std::cout << "Hello!\n";

    Graph g;
    Parser parser;

    parser.parse("samples/SNAP.3980.edges", g);

    // std::cout << g << "\n";

    std::cout << g.getV() << ", " << g.getEdgesCount() << "\n";

    std::cout << "Fin!\n";
    return 0;
}
