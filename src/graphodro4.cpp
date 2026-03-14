#include <iostream>

#include "_debugging.hpp"
#include "adjacency_list.hpp"
#include "parsers.hpp"

using Graph = AdjacencyList;
using Parser = EdgeListParser;

int main() {
    std::cout << "Hello!\n";

    Graph g;
    Parser parser;

    parser.parse("samples/edgelist1.txt", g);
    std::cout << g.getV() << " vertices, " << g.getEdgesCount() << " edges!\n";

    for (int i = 0; i < 5; i++) {
        std::cout << "deg(" << i << ")=" << g.deg(i) << "\tG[" << i << "]=";
        print_vector(g.at(i));
        std::cout << "\n";
    }

    std::cout << "Fin!\n";
    return 0;
}
