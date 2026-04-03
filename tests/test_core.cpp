#include <catch2/catch_all.hpp>
#include <graphodro4/core/adjacency_list.hpp>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <stdexcept>

TEMPLATE_TEST_CASE("IGraph: Core API Functionality", "[core][10pts]",
                   AdjacencyList, AdjacencyMatrix) {
    TestType g;

    SECTION("Memory Allocation and Vertices Count") {
        const size_t n = 10;
        g.reallocate(n);

        REQUIRE(g.getVerticesCount() == n);
        REQUIRE(g.getV() == n);  // Проверка псевдонима
        REQUIRE(g.getEdgesCount() == 0);
    }

    SECTION("Edge Operations: Add and Check") {
        g.reallocate(5);

        // Добавляем ребро и проверяем счетчик
        g.addEdge(0, 1);
        REQUIRE(g.getEdgesCount() == 1);
        REQUIRE(g.hasEdge(0, 1) == true);
        REQUIRE(g.hasEdge(1, 0) == true);  // Для неориентированного графа

        // Проверка отсутствующего ребра
        REQUIRE(g.hasEdge(0, 2) == false);
    }

    SECTION("Neighbors Integrity") {
        g.reallocate(4);
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(0, 3);

        auto neighbors = g.getNeighbors(0);
        REQUIRE(neighbors.size() == 3);

        // Проверка наличия всех индексов
        std::set<size_t> n_set(neighbors.begin(), neighbors.end());
        REQUIRE(n_set.count(1));
        REQUIRE(n_set.count(2));
        REQUIRE(n_set.count(3));
    }

    SECTION("Dynamic Expansion on addEdge") {
        g.reallocate(3);  // Начальный размер 3 (вершины 0, 1, 2)

        // Добавляем ребро к вершине 10.
        // Граф должен сам вырасти до размера 11, чтобы вместить индекс 10.
        g.addEdge(0, 10);

        REQUIRE(g.getVerticesCount() >= 11);
        REQUIRE(g.hasEdge(0, 10) == true);
        REQUIRE(g.getEdgesCount() == 1);

        // Проверяем, что getNeighbors теперь не кидает исключение для новой
        // вершины (так как она теперь валидна)
        CHECK_NOTHROW(g.getNeighbors(10));

        // А вот чтение соседей у совсем запредельной вершины всё еще может
        // или должно бросать out_of_range, если к ней не обращались через
        // addEdge
        CHECK_THROWS_AS(g.getNeighbors(100), std::out_of_range);
    }

    SECTION("Reallocation Safety") {
        g.reallocate(2);
        g.addEdge(0, 1);

        // Увеличиваем размер — данные (ребро 0-1) должны сохраняться
        g.reallocate(5);
        REQUIRE(g.getVerticesCount() == 5);
        REQUIRE(g.hasEdge(0, 1) == true);

        // Уменьшаем размер до 1 вершины — ребро (0,1) должно исчезнуть
        g.reallocate(1);
        REQUIRE(g.getVerticesCount() == 1);

        // 1. Счетчик ребер должен стать 0, так как вершина 1 удалена
        REQUIRE(g.getEdgesCount() == 0);

        // 2. Попытка проверить наличие ребра (0,1) должна кидать исключение,
        // так как индекс 1 теперь вне диапазона [0, 1)
        CHECK_THROWS_AS(g.hasEdge(0, 1), std::out_of_range);
    }
}

TEST_CASE("IGraph: Multiple Edges Handling", "[core][multigraph]") {
    AdjacencyList g;
    g.reallocate(2);

    g.addMultipleEdges(0, 1, 3);

    // В зависимости от логики: либо 3 ребра, либо 1 (если игнорируем дубликаты)
    REQUIRE(g.getEdgesCount() == 3);
}
