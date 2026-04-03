#ifndef DFS_HPP
#define DFS_HPP

#include <graphodro4/core/igraph.hpp>
#include <limits>
#include <set>
#include <vector>

class DFS {
   public:
    static constexpr size_t NoParent = std::numeric_limits<size_t>::max();

   private:
    const IUnweightedGraph& m_graph;

    // Вспомогательные структуры для алгоритма Тарьяна
    std::vector<int> m_tin;      // Time In
    std::vector<int> m_lowLink;  // Low-link
    int m_timer;

    // Результаты анализа
    std::set<size_t> m_articulationPoints;
    std::vector<std::pair<size_t, size_t>> m_bridges;
    size_t m_componentsCount;

    // Состояние для двудольности
    std::vector<int> m_colors;  // -1 = unvisited, 0/1 = colors
    bool m_isBipartite;

   public:
    explicit DFS(const IUnweightedGraph& graph);

    void compute();

    // Геттеры
    const std::set<size_t>& getArticulationPoints() const {
        return m_articulationPoints;
    }
    const std::vector<std::pair<size_t, size_t>>& getBridges() const {
        return m_bridges;
    }
    size_t getComponentsCount() const {
        return m_componentsCount;
    }
    bool isBipartite() const {
        return m_isBipartite;
    }

   private:
    void findStructures(size_t vertex, size_t parent = NoParent);
};

#endif
