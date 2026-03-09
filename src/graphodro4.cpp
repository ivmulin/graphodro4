#include <iostream>

#include "adjacency_list.hpp"
#include "parsers.hpp"

using Graph = AdjacencyList;

int main() {
    std::cout << "Hello!\n";
    /*
        Graph g;
        // g.addVertex(2);
        g.addEdge(0, 0);
        // g.addVertex();
        g.addEdge(1, 0);
        g.addEdge(1, 3);
        g.addEdge(3, 2);
        g.addEdge(2, 4);
        g.addEdge(1, 2);
    */
    Graph g;

    EdgeListParser elp;
    elp.parse("samples/edgelist1.txt", g);
    std::cout << g << "\n";

    g.addEdge(1, 2);
    g.addEdge(1, 2);

    std::cout << "Added two more edges (1, 2):\n" << g << "\n";
    std::cout << "deg(1) = " << g.deg(1) << "\n";

    g.rmEdges(2, 1);

    std::cout << "Removed edge (1, 2):\n" << g << "\n";

    g.rmEdges(1, 2, 10);

    std::cout << "Removed edges (1, 2):\n" << g << "\n";

    std::cout << "Fin!\n";
    return 0;
}
