#include <catch2/catch_all.hpp>
#include <cmath>
#include <graphodro4/algorithms/metrics.hpp>
#include <graphodro4/core/adjacency_list.hpp>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <graphodro4/generators/generator.hpp>
#include <numeric>

using Catch::Approx;

// Вспомогательный шаблон для тестирования разных структур данных
TEMPLATE_TEST_CASE("GraphFactory: Multi-backend verification",
                   "[generators][10pts]", AdjacencyList, AdjacencyMatrix) {
    TestType g;
    const size_t n = 6;

    SECTION("Complete Graph (Kn) - Density and Degrees") {
        GraphFactory::complete(g, n);
        GraphMetrics m(g);

        // Математический инвариант: плотность клики = 1.0
        REQUIRE(m.getDensity() == Approx(1.0));
        REQUIRE(g.getEdgesCount() == n * (n - 1) / 2);

        for (size_t i = 0; i < n; ++i) {
            REQUIRE(g.getNeighbors(i).size() == n - 1);
        }
    }

    SECTION("Star Graph - Random Center Invariant") {
        GraphFactory::star(g, n);

        size_t center_count = 0;
        size_t leaf_count = 0;

        for (size_t i = 0; i < n; ++i) {
            size_t deg = g.getNeighbors(i).size();
            if (deg == n - 1)
                center_count++;
            else if (deg == 1)
                leaf_count++;
        }

        // Проверяем топологию, а не конкретные индексы
        REQUIRE(center_count == 1);
        REQUIRE(leaf_count == n - 1);
    }
}

TEST_CASE("GraphFactory: Advanced Topological Structures",
          "[generators][advanced]") {
    AdjacencyList g;

    SECTION("Cubic Graph - 3-Regularity") {
        const size_t n = 10;  // n должно быть четным для кубического графа
        GraphFactory::cubic(g, n);

        bool is_cubic = true;
        for (size_t i = 0; i < n; ++i) {
            if (g.getNeighbors(i).size() != 3) is_cubic = false;
        }
        REQUIRE(is_cubic);
        REQUIRE(g.getEdgesCount() == (n * 3) / 2);
    }

    SECTION("Fixed Bridges - Connectivity constraints") {
        const size_t n = 15;
        const size_t k = 4;
        GraphFactory::fixedBridges(g, n, k);

        GraphMetrics m(g);
        // Проверка, что алгоритм генерации мостов отработал корректно
        REQUIRE(m.getBridgesCount() == k);
    }

    SECTION("Forest - Components and Edges ratio") {
        const size_t n = 12;
        const size_t k = 3;
        GraphFactory::forest(g, n, k);

        GraphMetrics m(g);
        // Инвариант леса: количество ребер = V - K
        REQUIRE(g.getEdgesCount() == n - k);
        REQUIRE(m.getComponentsCount() == k);
    }

    SECTION("Wheel Graph - Structural Invariants") {
        const size_t n = 7;
        GraphFactory::wheel(g, n);

        // Один центр (deg=n-1), остальные на ободе (deg=3)
        size_t deg_n_minus_1 = 0;
        size_t deg_3 = 0;
        for (size_t i = 0; i < n; ++i) {
            size_t d = g.getNeighbors(i).size();
            if (d == n - 1)
                deg_n_minus_1++;
            else if (d == 3)
                deg_3++;
        }
        REQUIRE(deg_n_minus_1 == 1);
        REQUIRE(deg_3 == n - 1);
    }
}

TEST_CASE("GraphFactory: Probability Distribution", "[generators][stats]") {
    AdjacencyList g;
    const size_t n = 60;
    const double p = 0.2;
    GraphFactory::random(g, n, p);

    double expected = p * (n * (n - 1) / 2.0);
    double variance = (n * (n - 1) / 2.0) * p * (1.0 - p);
    double std_dev = std::sqrt(variance);

    // Правило 3-х сигм для Erdos-Renyi
    REQUIRE(static_cast<double>(g.getEdgesCount()) >= expected - 3 * std_dev);
    REQUIRE(static_cast<double>(g.getEdgesCount()) <= expected + 3 * std_dev);
}
