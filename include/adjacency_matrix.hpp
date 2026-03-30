#ifndef ADJ_MATRIX_HPP
#define ADJ_MATRIX_HPP

#include <sys/types.h>

#include <cstddef>
#include <iostream>
#include <vector>

#include "igraph.hpp"

class AdjacencyMatrix : public IUnweightedGraph {
   private:
    /// @brief Количество вершин в графе
    size_t p_n = 0;
    /// @brief Количество ребер в графе
    size_t p_edges = 0;
    /// @brief Список смежности
    std::vector<size_t> p_adjMatrix;

    size_t p_cols = 0;
    size_t p_rows = 0;

   public:
    /**
     * @brief Получить список соседей вершины
     * @param vertex Индекс вершины
     * @return Вектор с индексами соседних вершин
     */
    std::vector<size_t> getNeighbors(size_t vertex) const override;

    /**
     * @brief Выделить память для вершин
     * @param n Количество вершин
     */
    void allocate(size_t n) override;

    /**
     * @brief Получить общее количество вершин
     * @return Количество вершин
     */
    size_t getVerticesCount() const override;

    /**
     * @brief Псевдоним для getVerticesCount()
     * @return Количество вершин
     */
    size_t getV() const override;

    /**
     * @brief Получить общее количество ребер
     * @return Количество ребер
     */
    size_t getEdgesCount() const override;

    /**
     * @brief Псевдоним для getEdgesCount()
     * @return Количество ребер
     */
    size_t getE() const override;

    /**
     * @brief Получить степень вершины (количество смежных ребер)
     * @param vertex Индекс вершины
     * @return Степень вершины
     */
    size_t deg(size_t vertex) const override;

    /**
     * @brief Оператор доступа к списку соседей вершины (без проверки границ)
     * @param vertex Индекс вершины
     * @return Константная ссылка на вектор соседей
     */
    const size_t* operator[](size_t vertex) const;

    size_t* operator[](size_t vertex);

    /**
     * @brief Метод доступа к списку соседей вершины (с проверкой границ)
     * @param vertex Индекс вершины
     * @return Константная ссылка на вектор соседей
     */
    const size_t* at(size_t vertex) const;

   public:
    /**
     * @brief Добавить ребро между двумя вершинами
     * @param from Индекс первой вершины
     * @param to Индекс второй вершины
     * @return Обновленное количество ребер в графе
     */
    size_t addEdge(size_t from, size_t to) override;

    /**
     * @brief Добавить k новых изолированных вершин в граф
     * @param k Количество добавляемых вершин
     * @return Обновленное общее количество вершин
     */
    size_t addVertices(size_t k = 1) override;

    /**
     * @brief Удалить ребра между двумя вершинами
     * @param from Индекс первой вершины
     * @param to Индекс второй вершины
     * @param occurrences Количество удаляемых вхождений ребра
     * @return Количество фактически удаленных ребер
     */
    size_t rmEdges(size_t from, size_t to, size_t occurrences = 1) override;

   private:
    /**
     * @brief Внутренний метод для вывода структуры списка смежности в поток
     * @param os Ссылка на поток вывода
     */
    void _print(std::ostream& os) const override;
};

#endif  // ADJACENCY_LIST_HPP
