#ifndef GRAPH_METRICS_HPP
#define GRAPH_METRICS_HPP

#include <cstdint>
#include <graphodro4/algorithms/dfs.hpp>
#include <graphodro4/core/igraph.hpp>
#include <optional>
#include <vector>

class GraphMetrics {
   private:
    const IUnweightedGraph& m_graph;

    // Структурный анализатор (DFS) для базовых метрик
    mutable DFS m_dfs;
    mutable bool m_dfsComputed = false;

    // Кэш для «тяжелых» характеристик
    mutable std::optional<double> m_density;
    mutable std::optional<size_t> m_diameter;
    mutable std::optional<double> m_transitivity;
    mutable std::optional<size_t> m_chromaticNumber;
    mutable std::optional<size_t> m_randomBridgesCount;

    // Вспомогательные внутренние методы
    void ensureDFS() const;
    size_t calculateDiameter() const;

    // Рекурсивная функция для XOR-хеширования мостов
    void computeRandomBridges(size_t v, size_t p,
                              std::vector<uint64_t>& vertexHashes,
                              std::vector<int>& visited, std::vector<int>& tin,
                              int& timer, size_t& bridgeCount) const;

   public:
    explicit GraphMetrics(const IUnweightedGraph& graph);

    // Метрики, делегируемые классу DFS
    size_t getComponentsCount() const;
    size_t getArticulationPointsCount() const;
    size_t getBridgesCount() const;  // Классический метод (Тарьян)
    bool isBipartite() const;

    // Метрики, вычисляемые в GraphMetrics
    double getDensity() const;
    size_t getDiameter() const;
    double getTransitivity() const;

    /**
     * @brief Оценка хроматического числа (жадная раскраска).
     * Дает верхнюю границу Chi(G).
     */
    size_t estimateChromaticNumber() const;

    /**
     * @brief Поиск мостов методом XOR-хеширования (Randomized).
     * Вероятностный алгоритм с использованием 64-битных меток.
     */
    size_t getBridgesCountRandomized() const;
};

#endif  // GRAPH_METRICS_HPP
