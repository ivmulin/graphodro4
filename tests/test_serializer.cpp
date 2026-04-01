#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "graphviz_serializer.hpp"
#include "adjacency_list.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <random>

using Graph = AdjacencyList;
using Serializer = GraphVizSerializer;

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

void saveToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create file: " + filename);
    }
    file << content;
    file.close();
}

Graph createSimpleGraph() {
    Graph g;
    g.addVertex(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);
    return g;
}

Graph createGraphWithCycle() {
    Graph g;
    g.addVertex(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 0);  // цикл
    g.addEdge(0, 3);  // хорда
    return g;
}

Graph createDisconnectedGraph() {
    Graph g;
    g.addVertex(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    return g;
}

TEST_CASE("Basic serialization - empty graph", "[serializer][basic]") {
    Graph g;
    g.addVertex(0);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Empty Graph");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("graph G {") != std::string::npos);
    REQUIRE(dot.find("label=\"Empty Graph\"") != std::string::npos);
}

TEST_CASE("Basic serialization - single vertex", "[serializer][basic]") {
    Graph g;
    g.addVertex(1);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Single Vertex");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("0") != std::string::npos);
}

TEST_CASE("Basic serialization - path graph", "[serializer][basic]") {
    Graph g;
    g.addVertex(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Path Graph");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("0 -- 1") != std::string::npos);
    REQUIRE(dot.find("1 -- 2") != std::string::npos);
    REQUIRE(dot.find("2 -- 3") != std::string::npos);
    
    // Проверка что нет дубликатов
    REQUIRE(dot.find("1 -- 0") == std::string::npos);
    REQUIRE(dot.find("2 -- 1") == std::string::npos);
}

TEST_CASE("Basic serialization - cycle graph", "[serializer][basic]") {
    Graph g = createSimpleGraph();
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Cycle C5");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("0 -- 1") != std::string::npos);
    REQUIRE(dot.find("1 -- 2") != std::string::npos);
    REQUIRE(dot.find("2 -- 3") != std::string::npos);
    REQUIRE(dot.find("3 -- 4") != std::string::npos);
    REQUIRE(dot.find("0 -- 4") != std::string::npos);
}

TEST_CASE("Basic serialization - complete graph K4", "[serializer][basic]") {
    Graph g;
    g.addVertex(4);
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = i + 1; j < 4; ++j) {
            g.addEdge(i, j);
        }
    }
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Complete K4");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    
    REQUIRE(dot.find("0 -- 1") != std::string::npos);
    REQUIRE(dot.find("0 -- 2") != std::string::npos);
    REQUIRE(dot.find("0 -- 3") != std::string::npos);
    REQUIRE(dot.find("1 -- 2") != std::string::npos);
    REQUIRE(dot.find("1 -- 3") != std::string::npos);
    REQUIRE(dot.find("2 -- 3") != std::string::npos);
}

TEST_CASE("File I/O - save and load", "[serializer][file]") {
    Graph g = createSimpleGraph();
    std::string filename = "test_io.dot";
    
    Serializer serializer(g);
    REQUIRE_NOTHROW(serializer.saveToFile(filename, "File IO Test"));
    
    REQUIRE(std::filesystem::exists(filename));
    
    std::string content = readFile(filename);
    REQUIRE(serializer.isValidDotFormat(content));
    REQUIRE(content.find("0 -- 1") != std::string::npos);
    
    cleanupTestFiles({filename});
}

TEST_CASE("File I/O - invalid path", "[serializer][file][error]") {
    Graph g;
    g.addVertex(2);
    
    std::string filename = "/nonexistent/path/test.dot";
    Serializer serializer(g);
    
    REQUIRE_THROWS_AS(serializer.saveToFile(filename), std::runtime_error);
}

TEST_CASE("File I/O - content consistency", "[serializer][file]") {
    Graph g = createGraphWithCycle();
    std::string filename = "test_consistency.dot";
    
    Serializer serializer(g);
    std::string serialized = serializer.serialize("Consistency Test");
    serializer.saveToFile(filename, "Consistency Test");
    std::string fromFile = readFile(filename);
    
    REQUIRE(serialized == fromFile);
    
    cleanupTestFiles({filename});
}

TEST_CASE("Vertex styling - colors", "[serializer][styling][vertex]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    
    std::map<size_t, VertexStyle> vStyles;
    vStyles[0] = {"red", "pink", "filled", "circle"};
    vStyles[1] = {"blue", "lightblue", "filled", "box"};
    vStyles[2] = {"green", "lightgreen", "filled", "doublecircle"};
    
    std::string dot = serializer.serializeFull("Vertex Colors", vStyles);
    
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
    REQUIRE(dot.find("fillcolor=\"pink\"") != std::string::npos);
    REQUIRE(dot.find("color=\"blue\"") != std::string::npos);
    REQUIRE(dot.find("fillcolor=\"lightblue\"") != std::string::npos);
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
    REQUIRE(dot.find("fillcolor=\"lightgreen\"") != std::string::npos);
}

TEST_CASE("Vertex styling - shapes", "[serializer][styling][vertex]") {
    Graph g;
    g.addVertex(4);
    
    Serializer serializer(g);
    
    std::map<size_t, VertexStyle> vStyles;
    vStyles[0] = {"black", "white", "filled", "circle"};
    vStyles[1] = {"black", "white", "filled", "box"};
    vStyles[2] = {"black", "white", "filled", "doublecircle"};
    vStyles[3] = {"black", "white", "filled", "diamond"};
    
    std::string dot = serializer.serializeFull("Vertex Shapes", vStyles);
    
    REQUIRE(dot.find("shape=circle") != std::string::npos);
    REQUIRE(dot.find("shape=box") != std::string::npos);
    REQUIRE(dot.find("shape=doublecircle") != std::string::npos);
    REQUIRE(dot.find("shape=diamond") != std::string::npos);
}

TEST_CASE("Vertex styling - partial", "[serializer][styling][vertex]") {
    Graph g;
    g.addVertex(3);
    
    Serializer serializer(g);
    
    std::map<size_t, VertexStyle> vStyles;
    vStyles[0] = {"red", "pink", "filled", "circle"};

    std::string dot = serializer.serializeFull("Partial Styling", vStyles);
    
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
    REQUIRE(serializer.isValidDotFormat(dot));
}

TEST_CASE("Edge styling - colors", "[serializer][styling][edge]") {
    Graph g;
    g.addVertex(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    
    Serializer serializer(g);
    
    std::map<std::pair<size_t, size_t>, EdgeStyle> eStyles;
    eStyles[{0, 1}] = {"red", "solid", 2.0};
    eStyles[{1, 2}] = {"blue", "dashed", 1.5};
    eStyles[{2, 3}] = {"green", "dotted", 3.0};
    
    std::string dot = serializer.serializeFull("Edge Colors", {}, eStyles);
    
    REQUIRE(dot.find("0 -- 1") != std::string::npos);
    REQUIRE(dot.find("1 -- 2") != std::string::npos);
    REQUIRE(dot.find("2 -- 3") != std::string::npos);
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
    REQUIRE(dot.find("color=\"blue\"") != std::string::npos);
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
}

TEST_CASE("Edge styling - styles", "[serializer][styling][edge]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    
    Serializer serializer(g);
    
    std::map<std::pair<size_t, size_t>, EdgeStyle> eStyles;
    eStyles[{0, 1}] = {"black", "solid", 1.0};
    eStyles[{1, 2}] = {"black", "dashed", 1.0};
    
    std::string dot = serializer.serializeFull("Edge Styles", {}, eStyles);
    
    REQUIRE(dot.find("style=solid") != std::string::npos);
    REQUIRE(dot.find("style=dashed") != std::string::npos);
}

TEST_CASE("Edge styling - penwidth", "[serializer][styling][edge]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    
    std::map<std::pair<size_t, size_t>, EdgeStyle> eStyles;
    eStyles[{0, 1}] = {"black", "solid", 5.0};
    
    std::string dot = serializer.serializeFull("Edge Width", {}, eStyles);
    
    REQUIRE(dot.find("penwidth=5") != std::string::npos);
}

TEST_CASE("Clusters - two components", "[serializer][clusters]") {
    Graph g = createDisconnectedGraph();
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> clusters = {{0, 1, 2}, {3, 4, 5}};
    
    std::string dot = serializer.serializeFull("Two Components", {}, {}, clusters);
    
    REQUIRE(dot.find("subgraph cluster_0") != std::string::npos);
    REQUIRE(dot.find("subgraph cluster_1") != std::string::npos);
    REQUIRE(dot.find("label=\"Component 0\"") != std::string::npos);
    REQUIRE(dot.find("label=\"Component 1\"") != std::string::npos);
    REQUIRE(dot.find("style=dashed") != std::string::npos);
}

TEST_CASE("Clusters - empty clusters", "[serializer][clusters]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> clusters = {};
    
    std::string dot = serializer.serializeFull("No Clusters", {}, {}, clusters);
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("subgraph cluster") == std::string::npos);
}

TEST_CASE("Clusters - single cluster", "[serializer][clusters]") {
    Graph g;
    g.addVertex(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> clusters = {{0, 1, 2, 3}};
    
    std::string dot = serializer.serializeFull("Single Cluster", {}, {}, clusters);
    
    REQUIRE(dot.find("subgraph cluster_0") != std::string::npos);
    REQUIRE(dot.find("0") != std::string::npos);
    REQUIRE(dot.find("3") != std::string::npos);
}

TEST_CASE("Clusters - with vertex styling", "[serializer][clusters]") {
    Graph g = createDisconnectedGraph();
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> clusters = {{0, 1, 2}, {3, 4, 5}};
    std::map<size_t, VertexStyle> vStyles;
    vStyles[0] = {"red", "pink", "filled", "circle"};
    vStyles[3] = {"blue", "lightblue", "filled", "box"};
    
    std::string dot = serializer.serializeFull("Clusters + Styling", vStyles, {}, clusters);
    
    REQUIRE(dot.find("subgraph cluster_0") != std::string::npos);
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
    REQUIRE(dot.find("color=\"blue\"") != std::string::npos);
}

TEST_CASE("Spanning tree - path", "[serializer][spanning-tree]") {
    Graph g;
    g.addVertex(5);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(0, 4);  
    
    Serializer serializer(g);
    
    std::vector<std::pair<size_t, size_t>> treeEdges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}
    };
    
    std::string dot = serializer.serializeFull("Spanning Tree", {}, {}, {}, treeEdges);
    
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
    REQUIRE(dot.find("penwidth=2.0") != std::string::npos);
}

TEST_CASE("Spanning tree - empty", "[serializer][spanning-tree]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    
    std::vector<std::pair<size_t, size_t>> treeEdges = {};
    
    std::string dot = serializer.serializeFull("No Tree", {}, {}, {}, treeEdges);
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("color=\"green\"") == std::string::npos);
}

TEST_CASE("Spanning tree - edge order normalization", "[serializer][spanning-tree]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    
    // Ребро указано в обратном порядке
    std::vector<std::pair<size_t, size_t>> treeEdges = {{1, 0}};
    
    std::string dot = serializer.serializeFull("Tree Order", {}, {}, {}, treeEdges);
    
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
}

TEST_CASE("Cycle - triangle", "[serializer][cycle]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    
    Serializer serializer(g);
    
    std::vector<size_t> cycle = {0, 1, 2};
    
    std::string dot = serializer.serializeFull("Triangle", {}, {}, {}, {}, cycle);
    
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
    REQUIRE(dot.find("penwidth=3.0") != std::string::npos);
    REQUIRE(dot.find("style=bold") != std::string::npos);
}

TEST_CASE("Cycle - square", "[serializer][cycle]") {
    Graph g;
    g.addVertex(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 0);
    
    Serializer serializer(g);
    
    std::vector<size_t> cycle = {0, 1, 2, 3};
    
    std::string dot = serializer.serializeFull("Square", {}, {}, {}, {}, cycle);
    
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
}

TEST_CASE("Cycle - empty", "[serializer][cycle]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    
    std::vector<size_t> cycle = {};
    
    std::string dot = serializer.serializeFull("No Cycle", {}, {}, {}, {}, cycle);
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("color=\"red\"") == std::string::npos);
}

TEST_CASE("Cycle - wrap around", "[serializer][cycle]") {
    Graph g;
    g.addVertex(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 0);
    
    Serializer serializer(g);
    
    std::vector<size_t> cycle = {0, 1, 2, 3};
    
    std::string dot = serializer.serializeFull("Cycle Wrap", {}, {}, {}, {}, cycle);
    
    // Ребро (3, 0) должно быть в цикле
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
}

TEST_CASE("Combined - all features", "[serializer][combined]") {
    Graph g;
    g.addVertex(6);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(2, 3);  
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> clusters = {{0, 1, 2}, {3, 4, 5}};
    
    std::vector<std::pair<size_t, size_t>> treeEdges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}
    };
    
    std::vector<size_t> cycle = {0, 1, 2};
    
    std::map<size_t, VertexStyle> vStyles;
    vStyles[0] = {"red", "pink", "filled", "doublecircle"};
    
    g.addEdge(0, 5);

    std::map<std::pair<size_t, size_t>, EdgeStyle> eStyles;
    eStyles[{0, 5}] = {"orange", "dashed", 2.0}; 
    
    std::string dot = serializer.serializeFull(
        "All Features", vStyles, eStyles, clusters, treeEdges, cycle
    );
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("subgraph cluster_0") != std::string::npos);
    REQUIRE(dot.find("subgraph cluster_1") != std::string::npos);
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
    REQUIRE(dot.find("color=\"red\"") != std::string::npos); 
    REQUIRE(dot.find("color=\"orange\"") != std::string::npos); 
}

TEST_CASE("Edge cases - self loop", "[serializer][edge-case]") {
    Graph g;
    g.addVertex(2);
    g.addEdge(0, 0);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Self Loop");
    
    REQUIRE(serializer.isValidDotFormat(dot));
}

TEST_CASE("Edge cases - multiple edges", "[serializer][edge-case]") {
    Graph g;
    g.addVertex(2);
    g.addEdge(0, 1);
    g.addEdge(0, 1);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Multi Edge");
    
    REQUIRE(serializer.isValidDotFormat(dot));
}

TEST_CASE("Edge cases - large graph", "[serializer][performance]") {
    Graph g;
    const size_t N = 100;
    g.addVertex(N);
    
    // Создаем путь
    for (size_t i = 0; i < N - 1; ++i) {
        g.addEdge(i, i + 1);
    }
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Large Graph");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("98 -- 99") != std::string::npos);
}

TEST_CASE("Edge cases - isolated vertices", "[serializer][edge-case]") {
    Graph g;
    g.addVertex(5);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Isolated");
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("4") != std::string::npos);
}

TEST_CASE("Lab visualization - random spanning tree", "[serializer][lab][visual]") {
    Graph g;
    g.addVertex(8);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.addEdge(6, 7);
    g.addEdge(0, 7);
    g.addEdge(1, 6);
    g.addEdge(2, 5);
    
    Serializer serializer(g);
    
    std::vector<std::pair<size_t, size_t>> treeEdges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}
    };
    
    std::string dot = serializer.serializeFull(
        "Random Spanning Tree", {}, {}, {}, treeEdges
    );
    
    saveToFile("visual_spanning_tree.dot", dot);
    
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
    REQUIRE(dot.find("penwidth=2.0") != std::string::npos);
}

TEST_CASE("Lab visualization - random cycle", "[serializer][lab][visual]") {
    Graph g;
    g.addVertex(7);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.addEdge(6, 0);
    g.addEdge(0, 3);
    g.addEdge(1, 4);
    
    Serializer serializer(g);
    
    // Случайный цикл
    std::vector<size_t> cycle = {0, 1, 2, 3, 4, 5, 6};
    
    std::string dot = serializer.serializeFull(
        "Random Cycle", {}, {}, {}, {}, cycle
    );
    
    saveToFile("visual_cycle.dot", dot);
    
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
    REQUIRE(dot.find("penwidth=3.0") != std::string::npos);
}

TEST_CASE("Lab visualization - components", "[serializer][lab][visual]") {
    Graph g;
    g.addVertex(8);

    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 0);

    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.addEdge(6, 7);
    g.addEdge(7, 4);
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> components = {
        {0, 1, 2, 3}, {4, 5, 6, 7}
    };
    
    std::string dot = serializer.serializeFull(
        "Connected Components", {}, {}, components
    );
    
    saveToFile("visual_components.dot", dot);
    
    REQUIRE(dot.find("subgraph cluster_0") != std::string::npos);
    REQUIRE(dot.find("subgraph cluster_1") != std::string::npos);
}

TEST_CASE("Lab visualization - biconnected components", "[serializer][lab][visual]") {
    Graph g;
    g.addVertex(7);

    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);  
    g.addEdge(2, 3);  
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    
    g.addEdge(5, 3);  
    g.addEdge(3, 6);
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> bicomponents = {
        {0, 1, 2}, {3, 4, 5}, {6}
    };
    
    std::string dot = serializer.serializeFull(
        "Biconnected Components", {}, {}, bicomponents
    );
    
    saveToFile("visual_bicomponents.dot", dot);
    
    REQUIRE(serializer.isValidDotFormat(dot));
}

TEST_CASE("Lab visualization - complete example", "[serializer][lab][visual]") {
    Graph g;
    g.addVertex(10);
    
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 0);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    
    g.addEdge(5, 6);
    g.addEdge(6, 7);
    g.addEdge(7, 8);
    g.addEdge(8, 9);
    
    g.addEdge(4, 5);
    
    Serializer serializer(g);
    
    std::vector<std::vector<size_t>> clusters = {
        {0, 1, 2, 3, 4}, {5, 6, 7, 8, 9}
    };
    
    std::vector<std::pair<size_t, size_t>> treeEdges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5},
        {5, 6}, {6, 7}, {7, 8}, {8, 9}
    };
    
    std::vector<size_t> cycle = {0, 1, 2, 3, 4};
    
    std::map<size_t, VertexStyle> vStyles;
    vStyles[4] = {"red", "yellow", "filled", "doublecircle"};  // точка сочленения
    vStyles[5] = {"blue", "lightblue", "filled", "box"};       // другая точка
    
    std::map<std::pair<size_t, size_t>, EdgeStyle> eStyles;
    eStyles[{4, 5}] = {"orange", "dashed", 3.0};  // мост
    
    std::string dot = serializer.serializeFull(
        "Complete Lab Example",
        vStyles,
        eStyles,
        clusters,
        treeEdges,
        cycle
    );
    
    saveToFile("visual_complete.dot", dot);
    
    REQUIRE(serializer.isValidDotFormat(dot));
    REQUIRE(dot.find("subgraph cluster_0") != std::string::npos);
    REQUIRE(dot.find("color=\"green\"") != std::string::npos);
    REQUIRE(dot.find("color=\"red\"") != std::string::npos);
}

TEST_CASE("Invariants - no duplicate edges", "[serializer][invariant]") {
    Graph g;
    g.addVertex(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(0, 3);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("No Duplicates");
    
    size_t count_0_1 = 0;
    size_t count_1_0 = 0;
    
    size_t pos = 0;
    while ((pos = dot.find("0 -- 1", pos)) != std::string::npos) {
        count_0_1++;
        pos += 6;
    }
    
    pos = 0;
    while ((pos = dot.find("1 -- 0", pos)) != std::string::npos) {
        count_1_0++;
        pos += 6;
    }
    
    REQUIRE(count_0_1 == 1);
    REQUIRE(count_1_0 == 0);
}

TEST_CASE("Invariants - all vertices present", "[serializer][invariant]") {
    Graph g;
    const size_t N = 6;
    g.addVertex(N);
    g.addEdge(0, 1);
    g.addEdge(2, 3);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("All Vertices");
    
    for (size_t i = 0; i < N; ++i) {
        std::string vertexStr = " " + std::to_string(i);
        REQUIRE(dot.find(vertexStr) != std::string::npos);
    }
}

TEST_CASE("Invariants - balanced braces", "[serializer][invariant]") {
    Graph g = createGraphWithCycle();
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Balanced");
    
    int braceCount = 0;
    for (char c : dot) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        REQUIRE(braceCount >= 0);
    }
    REQUIRE(braceCount == 0);
}

TEST_CASE("DOT format - valid structure", "[serializer][format]") {
    Graph g;
    g.addVertex(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Format Test");
    
    REQUIRE(dot.find("graph G {") == 0);
    REQUIRE(dot.back() == '\n');
    REQUIRE(dot.find("}\n") != std::string::npos);
}

TEST_CASE("DOT format - labels", "[serializer][format]") {
    Graph g;
    g.addVertex(2);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("My Test Graph");
    
    REQUIRE(dot.find("label=\"My Test Graph\"") != std::string::npos);
}

TEST_CASE("DOT format - node and edge defaults", "[serializer][format]") {
    Graph g;
    g.addVertex(2);
    g.addEdge(0, 1);
    
    Serializer serializer(g);
    std::string dot = serializer.serialize("Defaults");
    
    REQUIRE(dot.find("node [") != std::string::npos);
    REQUIRE(dot.find("edge [") != std::string::npos);
}