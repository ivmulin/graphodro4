#ifndef IGRAPH_HPP
#define IGRAPH_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

class IGraph {
    /* IGraph
     * Базовый интерфейс для графа
     *
     * Требуется для реализации интерфейсов
     */
   public:
    virtual ~IGraph() = default;

    virtual std::vector<size_t> getNeighbors(size_t vertex) const = 0;

    virtual size_t getVertexCount() const = 0;
    virtual size_t getV() const = 0;
    virtual size_t getEdgesCount() const = 0;
    virtual size_t getE() const = 0;

    virtual size_t deg(size_t vertex) const = 0;

    virtual size_t addVertex(uint8_t k = 1) = 0;
    virtual size_t addEdge(size_t from, size_t to) = 0;
    virtual size_t rmEdges(size_t from, size_t to, size_t occurrences = 1) = 0;

    // Перегрузка std::cout
    virtual void _print(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const IGraph& graph) {
        graph._print(os);  // Вызываем виртуальный метод
        return os;
    }
};

class IUnweightedGraph : public IGraph {
    /* IUnweightedGraph
     * Интерфейс для реализации невзвешенного графа
     */
   public:
};

#endif  // IGRAPH_HPP
