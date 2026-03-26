#include <iostream>
#include <vector>

#include "dfs.h"
#include "igraph.h"
#include "igraph.hpp"

class DFS {
   public:
    /**
     * @brief Выполнить обход в глубину
     * @param graph Граф для обхода
     * @param start Начальная вершина
     */
    DFS(const IUnweightedGraph& graph) : graph(graph) {
    }

    void dfs(const IUnweightedGraph& graph, size_t start) {
        std::vector<bool> visited(graph.vertexCount(), false);
        dfsUtil(graph, start, visited);
    }

   private:
    void dfsUtil(const IUnweightedGraph& graph, size_t vertex,
                 std::vector<bool>& visited) {
        visited[vertex] = true;

        for (size_t neighbor : graph.getNeighbors(vertex)) {
            if (!visited[neighbor]) {
                dfsUtil(graph, neighbor, visited);
            }
        }

        std::cout << vertex << " ";
    }
};
