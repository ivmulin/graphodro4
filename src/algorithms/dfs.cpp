#include <algorithm>
#include <graphodro4/algorithms/dfs.hpp>

DFS::DFS(const IUnweightedGraph& graph)
    : m_graph(graph), m_timer(0), m_componentsCount(0), m_isBipartite(true) {
}

void DFS::compute() {
    size_t n = m_graph.getVerticesCount();

    m_tin.assign(n, -1);
    m_lowLink.assign(n, -1);
    m_colors.assign(n, -1);

    m_timer = 0;
    m_componentsCount = 0;
    m_isBipartite = true;

    m_articulationPoints.clear();
    m_bridges.clear();

    for (size_t i = 0; i < n; ++i) {
        if (m_tin[i] == -1) {
            m_componentsCount++;
            m_colors[i] = 0;
            findStructures(i, NoParent);
        }
    }
}

void DFS::findStructures(size_t vertex, size_t parent) {
    m_tin[vertex] = m_lowLink[vertex] = m_timer++;
    int childrenCount = 0;
    bool parentEdgeSkipped = false;

    for (size_t neighbor : m_graph.getNeighbors(vertex)) {
        if (neighbor == parent && !parentEdgeSkipped) {
            parentEdgeSkipped = true;
            continue;
        }

        if (m_tin[neighbor] != -1) {
            // Обратное ребро
            m_lowLink[vertex] = std::min(m_lowLink[vertex], m_tin[neighbor]);

            if (m_colors[neighbor] == m_colors[vertex]) {
                m_isBipartite = false;
            }
        } else {
            // Ребро дерева DFS
            childrenCount++;
            m_colors[neighbor] = 1 - m_colors[vertex];

            findStructures(neighbor, vertex);

            m_lowLink[vertex] =
                std::min(m_lowLink[vertex], m_lowLink[neighbor]);

            if (m_lowLink[neighbor] > m_tin[vertex]) {
                m_bridges.push_back({vertex, neighbor});
            }

            if (m_lowLink[neighbor] >= m_tin[vertex] && parent != NoParent) {
                m_articulationPoints.insert(vertex);
            }
        }
    }

    if (parent == NoParent && childrenCount > 1) {
        m_articulationPoints.insert(vertex);
    }
}
