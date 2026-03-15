#ifndef IGRAPH_HPP
#define IGRAPH_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

/**
 * @brief Базовый интерфейс для графа.
 * Требуется для реализации интерфейсов.
 */
class IGraph {
   public:
    virtual ~IGraph() = default;

    /** @brief Возвращает количество вершин в графе */
    virtual size_t getVerticesCount() const = 0;

    /** @brief Возвращает количество вершин в графе */
    virtual size_t getV() const = 0;

    /** @brief Возвращает количество ребер в графе */
    virtual size_t getEdgesCount() const = 0;

    /** @brief Возвращает количество ребер в графе */
    virtual size_t getE() const = 0;

    /** @brief Добавляет k вершин к графу */
    virtual size_t addVertices(uint8_t k = 1) = 0;

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
    /** @brief Добавляет невзвешенное ребро */
    virtual size_t addEdge(size_t from, size_t to) = 0;

    /** @brief Удаляет невзвешенное ребро */
    virtual size_t rmEdges(size_t from, size_t to, size_t occurrences = 1) = 0;

    /** @brief Возвращает окрестность вершины */
    virtual std::vector<size_t> getNeighbors(size_t vertex) const = 0;

    /** @brief Получает степень вершины */
    virtual size_t deg(size_t vertex) const = 0;
};

/**
 * @brief Интерфейс для взвешенных графов
 * @tparam T Численный тип для весов (int, double и т.д).
 */
template <typename T>
class IWeightedGraph : virtual public IGraph {
   public:
    /** @brief Добавляет взвешенное ребро */
    virtual void addEdge(size_t from, size_t to, T weight) = 0;

    /** @brief Возвращает пару (сосед, вес). */
    virtual std::vector<std::pair<size_t, T>> getWeightedNeighbors(
        size_t vertex) const = 0;
};

#endif  // IGRAPH_HPP
