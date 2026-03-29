#include <cassert>
#include <graphodro4/algorithms/metrics.hpp>
#include <graphodro4/core/adjacency_list.hpp>
#include <iomanip>
#include <iostream>
#include <vector>

// Вспомогательная функция для вывода разделителя
void printSeparator() {
    std::cout << std::string(40, '-') << std::endl;
}

void test_complete_graph_k4() {
    std::cout << "[Test] Complete Graph (K4):" << std::endl;
    AdjacencyList g;
    size_t n = 4;
    g.reallocate(n);
    // Соединяем все вершины со всеми
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            g.addEdge(i, j);
        }
    }

    GraphMetrics metrics(g);

    // Плотность K4: 2*6 / (4*3) = 1.0
    assert(metrics.getDensity() == 1.0);
    // Транзитивность клики всегда 1.0
    assert(metrics.getTransitivity() == 1.0);
    // Хроматическое число K4 равно 4
    assert(metrics.estimateChromaticNumber() == 4);
    // В полных графах (n>2) мостов нет
    assert(metrics.getBridgesCount() == 0);
    assert(metrics.getBridgesCountRandomized() == 0);

    std::cout << "  - Density & Transitivity: OK (1.0)\n"
              << "  - Chromatic Number: OK (4)\n"
              << "  - XOR Bridges: OK (0)\n";
}

void test_bridge_structure() {
    std::cout << "[Test] Bridge Structure (0-1, 1-2, 2-0, 2-3):" << std::endl;
    AdjacencyList g;
    g.reallocate(4);
    // Треугольник 0-1-2
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    // Ребро-мост к вершине 3
    g.addEdge(2, 3);

    GraphMetrics metrics(g);

    // Классический и XOR методы должны вернуть 1 мост (ребро 2-3)
    size_t standardBridges = metrics.getBridgesCount();
    size_t randomBridges = metrics.getBridgesCountRandomized();

    assert(standardBridges == 1);
    assert(randomBridges == 1);
    // Точка сочленения — вершина 2
    assert(metrics.getArticulationPointsCount() == 1);
    // Диаметр: от 0 (или 1) до 3 путь равен 2
    assert(metrics.getDiameter() == 2);

    std::cout << "  - Standard Bridges: " << standardBridges << " (OK)\n"
              << "  - Randomized Bridges: " << randomBridges << " (OK)\n"
              << "  - Diameter: OK (2)\n";
}

void test_bipartite_and_chromatic() {
    std::cout << "[Test] Bipartite Star Graph (Center 0, Leaves 1,2,3):"
              << std::endl;
    AdjacencyList g;
    g.reallocate(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);

    GraphMetrics metrics(g);

    // Звезда всегда двудольна
    assert(metrics.isBipartite() == true);
    // Хроматическое число для двудольного графа должно быть 2
    assert(metrics.estimateChromaticNumber() == 2);
    // Транзитивность звезды 0.0 (нет треугольников)
    assert(metrics.getTransitivity() == 0.0);

    std::cout << "  - Bipartite: OK\n"
              << "  - Chromatic Number: OK (2)\n"
              << "  - Transitivity: OK (0.0)\n";
}

int main() {
    std::cout << std::fixed << std::setprecision(3);

    try {
        test_complete_graph_k4();
        printSeparator();
        test_bridge_structure();
        printSeparator();
        test_bipartite_and_chromatic();

        std::cout << "\n[ALL TESTS PASSED] GraphMetrics is reliable."
                  << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] Test failed: " << e.what()
                  << std::endl;
        return 1;
    }

    return 0;
}
