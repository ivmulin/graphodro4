#include <iostream>

#include "_debugging.hpp"
#include "adjacency_list.hpp"
#include "parsers.hpp"
#include "graphviz_serializer.hpp"

using Graph = AdjacencyList;
using Parser = EdgeListParser;
using Serializer = GraphVizSerializer;


int main() {
    std::cout << "Hello!\n";

    Graph g;
    Parser parser;

    parser.parse("../samples/edgelist1.txt", g);
    std::cout << g.getV() << " vertices, " << g.getEdgesCount() << " edges!\n";

    for (int i = 0; i < 5; i++) {
        std::cout << "deg(" << i << ")=" << g.deg(i) << "\tG[" << i << "]=";
        print_vector(g.at(i));
        std::cout << "\n";
    }

    std::cout << "Fin!\n";

    try {
        Graph g;
        Parser{}.parse("../samples/edgelist1.txt", g);

        std::cout << "Loaded: " << g.getV() << " vertices, " 
                  << g.getEdgesCount() << " edges\n";

        Serializer(g).saveToFile("output.dot", "Graph");
        std::cout << "Saved to output.dot\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
