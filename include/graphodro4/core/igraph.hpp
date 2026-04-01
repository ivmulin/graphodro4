#ifndef IGRAPH_HPP
#define IGRAPH_HPP

#include <cstddef>
#include <functional>
#include <iostream>
#include <vector>

/**
 * @brief Базовый интерфейс для графа.
 * Требуется для реализации интерфейсов.
 */
class IGraph {
   private:
    size_t p_n;
    size_t p_edges;

   public:
    virtual ~IGraph() = default;

    virtual void reallocate(size_t n) = 0;

    virtual void allocate(size_t n) = 0;

    /** @brief Возвращает количество вершин в графе */
    virtual size_t getVerticesCount() const = 0;

    /** @brief Возвращает количество вершин в графе */
    virtual size_t getV() const = 0;

    /** @brief Возвращает количество ребер в графе */
    virtual size_t getEdgesCount() const = 0;

    /** @brief Возвращает количество ребер в графе */
    virtual size_t getE() const = 0;

    virtual bool hasEdge(size_t from, size_t to) const = 0;

    /** @brief Возвращает окрестность вершины */
    virtual std::vector<size_t> getNeighbors(size_t vertex) const = 0;

    /** @brief Добавляет невзвешенное ребро */
    virtual size_t addEdge(size_t from, size_t to) = 0;

    size_t addMultipleEdges(size_t from, size_t to, size_t count) {
        for (size_t i = 0; i < count; i++) {
            addEdge(from, to);
        }
        return p_edges;
    }

    /** @brief Добавляет k вершин к графу */
    virtual size_t addVertices(size_t k = 1) = 0;

    virtual void forEachEdge(
        std::function<void(size_t, size_t)> callback) const = 0;

    /** @brief Метод, определяющий, как конкретная реализация наследников будет
     * отображаеться в консоли */
    virtual void _print(std::ostream& os) const = 0;

    /** @brief Перегрузка оператора<< стандартного вывода */
    friend std::ostream& operator<<(std::ostream& os, const IGraph& graph) {
        graph._print(os);  // Вызываем виртуальный метод
        return os;
    }
};

/**
 * @brief Интерфейс для невзвешенных графов.
 */
class IUnweightedGraph : virtual public IGraph {
    /* IUnweightedGraph
     * Интерфейс для реализации невзвешенного графа
     */
   public:
    /** @brief Удаляет невзвешенное ребро */
    virtual size_t rmEdges(size_t from, size_t to, size_t occurrences = 1) = 0;

    /** @brief Получает степень вершины */
    virtual size_t deg(size_t vertex) const = 0;
};

#endif  // IGRAPH_HPP
