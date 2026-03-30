#include <iostream>

#include "graphs.hpp"
#include "parsers.hpp"
#include "graphviz_serializer.hpp"

using Graph = AdjacencyList;
using Parser = EdgeListParser;
using Serializer = GraphVizSerializer;

int main() {
    std::cout << "Hello!\n";

    Graph g;
    ParserUtility parser(CurrentParser::SNAP);

    parser.parse("../samples/edgelist1.txt", g);
    std::cout << g.getV() << " vertices, " << g.getEdgesCount() << " edges!\n";

    // std::cout << g << "\n";

    std::cout << g.getV() << ", " << g.getEdgesCount() << "\n";

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
