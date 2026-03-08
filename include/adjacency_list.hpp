#ifndef ADJACENCY_LIST_HPP
#define ADJACENCY_LIST_HPP

#include <sys/types.h>

#include <cstdint>
#include <iostream>
#include <vector>

#include "igraph.hpp"

class AdjacencyList : public IUnweightedGraph {
   private:
    size_t p_n = 0;
    size_t p_edges = 0;
    std::vector<std::vector<int>> p_adjList;

   public:
    // геттеры
    std::vector<int> getNeighbors(int vertex) const override;
    size_t getVertexCount() const override;
    size_t getN() const override;

   public:
    // модификаторы
    int addEdge(int from, int to) override;
    int addVertex(uint8_t k = 1) override;

   public:
    // всякое
    void _print(std::ostream& os) const override;
};

#endif  // ADJACENCY_LIST_HPP
