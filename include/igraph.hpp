#ifndef IGRAPH_HPP
#define IGRAPH_HPP

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
    virtual int getVertexCount() const = 0;
    virtual int getN() const = 0;
};

class IUnweightedGraph : public IGraph {
    /* IUnweightedGraph
     * Интерфейс для реализации невзвешенного графа
     */
   public:
    virtual void addEdge(int from, int to) = 0;
};

#endif  // IGRAPH_HPP
