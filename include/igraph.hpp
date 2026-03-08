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

    virtual std::vector<int> getNeighbors(int vertex) const = 0;
    virtual size_t getVertexCount() const = 0;
    virtual size_t getN() const = 0;

    virtual int addVertex(uint8_t k = 1) = 0;
    virtual int addEdge(int from, int to) = 0;

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
