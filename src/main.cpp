#include <graphodro4/core/graphs.hpp>
#include <graphodro4/parsers/parsers.hpp>

#include "graphodro4/generators/generator.hpp"

using Graph = AdjacencyList;

int main() {
    Graph g;
    EdgeListParser parser;

    // ЭТО ВСЕ ДЛЯ ЛИНУКСА РАБОТАЕТ!!!!!!!!!!!!!

    size_t uganda = 1;

    if (uganda == 1) {
        // Запуск откуда угодно
        std::cout << "Running from wherever\n";
        parser.parse("/home/ino/dev/graphodro4/samples/EDGE_LIST.b.txt", g);
    } else if (uganda == 2) {
        // Запуск из корня проекта
        std::cout << "Running from root\n";
        parser.parse("samples/EDGE_LIST.b.txt", g);
    } else if (uganda == 3) {
        // Запуск из build
        std::cout << "Running from build\n";
        parser.parse("../samples/EDGE_LIST.b.txt", g);
    } else {
        std::cout << "Go play ca sin(n) o\n Enough coding for today\n";
        return 0;
    }

    // std::cout << g << "\n";

    g.addEdge(3, 4);

    g.addVertices(5);

    // std::cout << g << "\n";

    GraphFactory::tree(g, 10);

    std::cout << g << "\n";
    return 0;
}
