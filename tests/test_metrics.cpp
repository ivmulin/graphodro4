#include <catch2/catch_all.hpp>
#include <graphodro4/algorithms/metrics.hpp>
#include <graphodro4/core/adjacency_list.hpp>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <graphodro4/generators/generator.hpp>

using Catch::Approx;

TEMPLATE_TEST_CASE("GraphMetrics: Core mathematical invariants",
                   "[metrics][10pts]", AdjacencyList, AdjacencyMatrix) {
    TestType g;

    SECTION("Density of standard structures") {
        // Kn: Плотность должна быть 1.0
        GraphFactory::complete(g, 5);
        GraphMetrics m1(g);
        REQUIRE(m1.getDensity() == Approx(1.0));

        // Пустой граф: Плотность 0.0
        g.allocate(10);
        GraphMetrics m2(g);
        REQUIRE(m2.getDensity() == Approx(0.0));
    }

    SECTION("Connectivity and Components") {
        // Дискретный граф (n вершин, 0 ребер) -> n компонент
        const size_t n = 8;
        g.reallocate(n);
        GraphMetrics m1(g);
        REQUIRE(m1.getComponentsCount() == n);
        // Вместо isConnected() проверяем, что компонент больше одной
        REQUIRE(m1.getComponentsCount() != 1);

        // Путь Pn -> 1 компонента (связный граф)
        GraphFactory::path(g, n);
        GraphMetrics m2(g);
        REQUIRE(m2.getComponentsCount() == 1);
    }

    SECTION("Bridges and Articulation Points") {
        // В дереве (пути) все ребра — мосты, а внутренние вершины — точки
        // сочленения
        const size_t n = 5;
        GraphFactory::path(g, n);
        GraphMetrics m(g);

        REQUIRE(m.getBridgesCount() == n - 1);
        REQUIRE(m.getArticulationPointsCount() == n - 2);
    }

    SECTION("Bipartiteness (Cycle check)") {
        // Четный цикл — двудольный
        GraphFactory::cycle(g, 4);
        GraphMetrics m1(g);
        REQUIRE(m1.isBipartite() == true);

        // Нечетный цикл — не двудольный
        g.reallocate(0);  // Сброс
        GraphFactory::cycle(g, 3);
        GraphMetrics m2(g);
        REQUIRE(m2.isBipartite() == false);
    }
}

TEST_CASE("GraphMetrics: Edge cases", "[metrics][robustness]") {
    AdjacencyList g;

    SECTION("Single vertex graph") {
        g.reallocate(1);
        GraphMetrics m(g);
        REQUIRE(m.getDensity() == Approx(0.0));
        REQUIRE(m.getComponentsCount() == 1);
        REQUIRE(m.getBridgesCount() == 0);
        REQUIRE(m.getArticulationPointsCount() == 0);
    }

    SECTION("Disconnected stars") {
        // Две отдельные звезды по 4 вершины каждая
        // Итого: 8 вершин, 2 компоненты, 6 мостов (3+3), 2 точки сочленения
        // (центры)
        g.reallocate(8);
        // Первая звезда (центр 0, лучи 1,2,3)
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(0, 3);
        // Вторая звезда (центр 4, лучи 5,6,7)
        g.addEdge(4, 5);
        g.addEdge(4, 6);
        g.addEdge(4, 7);

        GraphMetrics m(g);
        REQUIRE(m.getComponentsCount() == 2);
        REQUIRE(m.getBridgesCount() == 6);
        REQUIRE(m.getArticulationPointsCount() == 2);
    }
}
