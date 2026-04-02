#include <graphodro4/core/graphs.hpp>
#include <graphodro4/parsers/parsers.hpp>
#include <graphodro4/serializer/graphviz_serializer.hpp>
#include <iostream>

using Graph = AdjacencyList;
using Parser = EdgeListParser;
using Serializer = GraphVizSerializer;

#define ROOT "/home/ino/dev/graphodro4"

int main() {
    std::cout << "Hello!\n";

    Graph g;
    ParserUtility parser(CurrentParser::SNAP);

    parser.parse(ROOT "/samples/EDGE_LIST.b.txt", g);
    std::cout << g.getV() << " vertices, " << g.getEdgesCount() << " edges!\n";

    // std::cout << g << "\n";

    std::cout << g.getV() << ", " << g.getEdgesCount() << "\n";

    std::cout << "Fin!\n";

    try {
        Graph g;
        Parser{}.parse(ROOT "/samples/EDGE_LIST.b.txt", g);

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
