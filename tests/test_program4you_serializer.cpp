#include <catch2/catch_all.hpp>
#include <graphodro4/core/adjacency_list.hpp>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <graphodro4/serializer/program4you_serializer.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

using Graph = AdjacencyList;
using Serializer = Program4YouSerializer;

void cleanupTestFiles(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        std::remove(file.c_str());
    }
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST_CASE("Program4You: Basic serialization - empty graph", "[program4you][basic]") {
    Graph g;
    g.allocate(0);
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(edges.find("0 0") != std::string::npos);
    REQUIRE(serializer.isValidFormat(edges));
}

TEST_CASE("Program4You: Basic serialization - single vertex", "[program4you][basic]") {
    Graph g;
    g.allocate(1);
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(edges.find("1 0") != std::string::npos);
    REQUIRE(serializer.isValidFormat(edges));
}

TEST_CASE("Program4You: Basic serialization - path graph", "[program4you][basic]") {
    Graph g;
    g.allocate(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(serializer.isValidFormat(edges));
    REQUIRE(edges.find("4 3") != std::string::npos);  // 4 вершины, 3 ребра
    REQUIRE(edges.find("0 1") != std::string::npos);
    REQUIRE(edges.find("1 2") != std::string::npos);
    REQUIRE(edges.find("2 3") != std::string::npos);
}

TEST_CASE("Program4You: Basic serialization - cycle graph", "[program4you][basic]") {
    Graph g;
    g.allocate(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(serializer.isValidFormat(edges));
    REQUIRE(edges.find("5 5") != std::string::npos);  // 5 вершин, 5 ребер
}

TEST_CASE("Program4You: Basic serialization - complete graph K4", "[program4you][basic]") {
    Graph g;
    g.allocate(4);
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = i + 1; j < 4; ++j) {
            g.addEdge(i, j);
        }
    }
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(serializer.isValidFormat(edges));
    REQUIRE(edges.find("4 6") != std::string::npos);  // K4 имеет 6 ребер
}

TEST_CASE("Program4You: File I/O - save and load", "[program4you][file]") {
    Graph g;
    g.allocate(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    
    std::string filename = "test_program4you.edges";
    Serializer serializer(g);
    
    REQUIRE_NOTHROW(serializer.saveToFile(filename));
    REQUIRE(std::filesystem::exists(filename));
    
    // Загружаем граф обратно
    Graph loaded_graph;
    Serializer::loadFromFile(filename, loaded_graph);
    
    REQUIRE(loaded_graph.getV() == g.getV());
    REQUIRE(loaded_graph.getE() == g.getE());
    
    // Проверяем наличие всех ребер
    REQUIRE(loaded_graph.hasEdge(0, 1));
    REQUIRE(loaded_graph.hasEdge(1, 2));
    REQUIRE(loaded_graph.hasEdge(2, 3));
    REQUIRE(loaded_graph.hasEdge(3, 4));
    
    cleanupTestFiles({filename});
}

TEST_CASE("Program4You: File I/O - invalid path", "[program4you][file][error]") {
    Graph g;
    g.allocate(2);
    g.addEdge(0, 1);
    
    std::string filename = "/nonexistent/path/test.edges";
    Serializer serializer(g);
    
    REQUIRE_THROWS_AS(serializer.saveToFile(filename), std::runtime_error);
}

TEST_CASE("Program4You: File I/O - content consistency", "[program4you][file]") {
    Graph g;
    g.allocate(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 0);
    
    std::string filename = "test_consistency.edges";
    Serializer serializer(g);
    
    std::string serialized = serializer.serialize();
    serializer.saveToFile(filename);
    std::string fromFile = readFile(filename);
    
    REQUIRE(serialized == fromFile);
    
    cleanupTestFiles({filename});
}

TEST_CASE("Program4You: Parse from string", "[program4you][parse]") {
    std::string content = "5 6\n0 1\n1 2\n2 3\n3 4\n4 0\n0 2\n";
    
    Graph g;
    Serializer::parse(content, g);
    
    REQUIRE(g.getV() == 5);
    REQUIRE(g.getE() == 6);
    REQUIRE(g.hasEdge(0, 1));
    REQUIRE(g.hasEdge(1, 2));
    REQUIRE(g.hasEdge(0, 2));
}

TEST_CASE("Program4You: Parse invalid format", "[program4you][parse][error]") {
    std::string invalid_content = "invalid header\n";
    
    Graph g;
    REQUIRE_THROWS_AS(Serializer::parse(invalid_content, g), std::runtime_error);
}

TEST_CASE("Program4You: No duplicate edges", "[program4you][invariant]") {
    Graph g;
    g.allocate(4);
    g.addEdge(0, 1);
    g.addEdge(1, 0);  // То же самое ребро (неориентированный граф)
    g.addEdge(1, 2);
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    // Считаем количество строк с ребрами
    size_t edge_count = 0;
    std::istringstream iss(edges);
    std::string line;
    std::getline(iss, line);  // Пропускаем заголовок
    
    while (std::getline(iss, line)) {
        if (!line.empty()) edge_count++;
    }
    
    REQUIRE(edge_count == 2);  // Должно быть 2 уникальных ребра
}

TEST_CASE("Program4You: Edge order normalization", "[program4you][invariant]") {
    Graph g;
    g.allocate(3);
    g.addEdge(2, 1);  // Ребро в обратном порядке
    g.addEdge(0, 2);
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    // Проверяем, что ребра записаны в нормализованном виде (меньший индекс первым)
    REQUIRE(edges.find("1 2") != std::string::npos);
    REQUIRE(edges.find("0 2") != std::string::npos);
}

TEST_CASE("Program4You: Round-trip conversion", "[program4you][integration]") {
    // Создаем исходный граф
    Graph original;
    original.allocate(8);
    original.addEdge(0, 1);
    original.addEdge(1, 2);
    original.addEdge(2, 3);
    original.addEdge(3, 4);
    original.addEdge(4, 5);
    original.addEdge(5, 6);
    original.addEdge(6, 7);
    original.addEdge(0, 7);
    original.addEdge(1, 5);
    original.addEdge(2, 6);
    
    // Сериализуем
    Serializer serializer(original);
    std::string serialized = serializer.serialize();
    
    // Парсим обратно
    Graph restored;
    Serializer::parse(serialized, restored);
    
    // Проверяем идентичность
    REQUIRE(restored.getV() == original.getV());
    REQUIRE(restored.getE() == original.getE());
    
    // Проверяем все ребра
    for (size_t u = 0; u < original.getV(); ++u) {
        for (size_t v : original.getNeighbors(u)) {
            REQUIRE(restored.hasEdge(u, v));
        }
    }
}

TEST_CASE("Program4You: Large graph", "[program4you][performance]") {
    Graph g;
    const size_t N = 100;
    g.allocate(N);
    
    // Создаем полный граф K100
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            g.addEdge(i, j);
        }
    }
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(serializer.isValidFormat(edges));
    
    // Проверяем количество ребер: n*(n-1)/2 = 100*99/2 = 4950
    REQUIRE(edges.find("100 4950") != std::string::npos);
}

TEST_CASE("Program4You: Graph with isolated vertices", "[program4you][edge-case]") {
    Graph g;
    g.allocate(5);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    // Вершина 4 изолирована
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(serializer.isValidFormat(edges));
    REQUIRE(edges.find("5 2") != std::string::npos);  // 5 вершин, 2 ребра
}

TEST_CASE("Program4You: Self-loop handling", "[program4you][edge-case]") {
    Graph g;
    g.allocate(3);
    g.addEdge(0, 0);  // Петля
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    REQUIRE(serializer.isValidFormat(edges));
}

TEST_CASE("Program4You: Multiple edges handling", "[program4you][edge-case]") {
    Graph g;
    g.allocate(2);
    g.addEdge(0, 1);
    g.addEdge(0, 1);  // Кратное ребро
    g.addEdge(0, 1);  // Еще одно кратное ребро
    
    Serializer serializer(g);
    std::string edges = serializer.serialize();
    
    // В неориентированном графе кратные ребра должны быть записаны один раз
    REQUIRE(serializer.isValidFormat(edges));
}

TEST_CASE("Program4You: Different backends", "[program4you][backend]") {
    // Тестируем с AdjacencyList
    AdjacencyList list_graph;
    list_graph.allocate(4);
    list_graph.addEdge(0, 1);
    list_graph.addEdge(1, 2);
    list_graph.addEdge(2, 3);
    
    Serializer list_serializer(list_graph);
    std::string list_edges = list_serializer.serialize();
    
    // Тестируем с AdjacencyMatrix
    AdjacencyMatrix matrix_graph;
    matrix_graph.allocate(4);
    matrix_graph.addEdge(0, 1);
    matrix_graph.addEdge(1, 2);
    matrix_graph.addEdge(2, 3);
    
    Serializer matrix_serializer(matrix_graph);
    std::string matrix_edges = matrix_serializer.serialize();
    
    // Обе сериализации должны быть валидными
    REQUIRE(list_serializer.isValidFormat(list_edges));
    REQUIRE(matrix_serializer.isValidFormat(matrix_edges));
    
    // И должны содержать одинаковое количество вершин и ребер
    REQUIRE(list_edges.find("4 3") != std::string::npos);
    REQUIRE(matrix_edges.find("4 3") != std::string::npos);
}