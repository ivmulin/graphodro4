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
    std::vector<std::vector<size_t>> p_adjList;

   public:
    // геттеры
    std::vector<size_t> getNeighbors(size_t vertex) const override;
    size_t getVertexCount() const override;
    size_t getN() const override;
    size_t deg(size_t vertex) const override;

   public:
    // модификаторы
    size_t addEdge(size_t from, size_t to) override;
    size_t addVertex(uint8_t k = 1) override;
    size_t rmEdges(size_t from, size_t to, size_t occurrences = 1) override;

   public:
    // всякое
    void _print(std::ostream& os) const override;
};

#endif  // ADJACENCY_LIST_HPP
