#include <iostream>
#include <vector>

#include "dfs.hpp"

DFS::DFS(const IUnweightedGraph& graph) : m_graph(graph) {
}

void DFS::dfs(size_t start) {
    std::vector<bool> visited(m_graph.getVerticesCount(), false);
    dfsUtil(m_graph, start, visited);
}

void DFS::dfsUtil(const IUnweightedGraph& graph, size_t vertex,
                  std::vector<bool>& visited) {
    visited[vertex] = true;

    for (size_t neighbor : graph.getNeighbors(vertex)) {
        if (!visited[neighbor]) {
            dfsUtil(graph, neighbor, visited);
        }
    }

    std::cout << vertex << " ";
}
