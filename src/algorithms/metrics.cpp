#include <algorithm>
#include <graphodro4/algorithms/metrics.hpp>
#include <queue>
#include <random>

GraphMetrics::GraphMetrics(const IUnweightedGraph& graph)
    : m_graph(graph), m_dfs(graph) {
}

void GraphMetrics::ensureDFS() const {
    if (!m_dfsComputed) {
        m_dfs.compute();
        m_dfsComputed = true;
    }
}

size_t GraphMetrics::getComponentsCount() const {
    ensureDFS();
    return m_dfs.getComponentsCount();
}

size_t GraphMetrics::getArticulationPointsCount() const {
    ensureDFS();
    return m_dfs.getArticulationPoints().size();
}

size_t GraphMetrics::getBridgesCount() const {
    ensureDFS();
    return m_dfs.getBridges().size();
}

bool GraphMetrics::isBipartite() const {
    ensureDFS();
    return m_dfs.isBipartite();
}

double GraphMetrics::getDensity() const {
    if (!m_density) {
        size_t v = m_graph.getVerticesCount();
        size_t e = m_graph.getEdgesCount();

        if (v < 2) {
            m_density = 0.0;
        } else {
            // Формула: 2|E| / (|V| * (|V| - 1))
            m_density = static_cast<double>(2 * e) / (v * (v - 1));
        }
    }
    return *m_density;
}

size_t GraphMetrics::getDiameter() const {
    if (!m_diameter) {
        m_diameter = calculateDiameter();
    }
    return *m_diameter;
}

size_t GraphMetrics::calculateDiameter() const {
    size_t n = m_graph.getVerticesCount();
    if (n == 0) return 0;

    size_t maxDistance = 0;

    // Для нахождения диаметра запускаем BFS из каждой вершины
    // Сложность: O(V * (V + E))
    for (size_t i = 0; i < n; ++i) {
        std::vector<int> dist(n, -1);
        std::queue<size_t> q;

        dist[i] = 0;
        q.push(i);

        while (!q.empty()) {
            size_t v = q.front();
            q.pop();

            maxDistance = std::max(maxDistance, static_cast<size_t>(dist[v]));

            for (size_t neighbor : m_graph.getNeighbors(v)) {
                if (dist[neighbor] == -1) {
                    dist[neighbor] = dist[v] + 1;
                    q.push(neighbor);
                }
            }
        }
    }

    return maxDistance;
}

double GraphMetrics::getTransitivity() const {
    if (!m_transitivity) {
        size_t trianglesCount = 0;
        size_t tripletsCount = 0;
        size_t n = m_graph.getVerticesCount();

        for (size_t v = 0; v < n; ++v) {
            auto neighbors = m_graph.getNeighbors(v);
            size_t k = neighbors.size();

            // Количество триад с центром в v: C(k, 2) = k * (k - 1) / 2
            if (k >= 2) {
                tripletsCount += k * (k - 1) / 2;
            }

            // Ищем замыкающие ребра между соседями
            for (size_t i = 0; i < k; ++i) {
                for (size_t j = i + 1; j < k; ++j) {
                    if (m_graph.hasEdge(neighbors[i], neighbors[j])) {
                        trianglesCount++;
                    }
                }
            }
        }

        if (tripletsCount == 0) {
            m_transitivity = 0.0;
        } else {
            // Так как каждый треугольник посчитан 3 раза (по разу для каждой
            // вершины), формула (3 * RealTriangles) / Triplets превращается в:
            m_transitivity =
                static_cast<double>(trianglesCount) / tripletsCount;
        }
    }
    return *m_transitivity;
}

size_t GraphMetrics::estimateChromaticNumber() const {
    size_t n = m_graph.getVerticesCount();
    if (n == 0) return 0;

    std::vector<int> result(n, -1);
    result[0] = 0;  // Назначаем первый цвет первой вершине

    std::vector<bool> available(n, false);

    for (size_t u = 1; u < n; u++) {
        // Помечаем цвета соседей как недоступные
        for (size_t neighbor : m_graph.getNeighbors(u)) {
            if (result[neighbor] != -1) {
                available[result[neighbor]] = true;
            }
        }

        // Ищем первый свободный цвет
        int color;
        for (color = 0; color < static_cast<int>(n); color++) {
            if (!available[color]) break;
        }

        result[u] = color;

        // Сбрасываем значения для следующей итерации
        for (size_t neighbor : m_graph.getNeighbors(u)) {
            if (result[neighbor] != -1) {
                available[result[neighbor]] = false;
            }
        }
    }

    // Максимальный номер цвета + 1
    int maxColor = 0;
    for (size_t i = 0; i < n; i++) {
        maxColor = std::max(maxColor, result[i]);
    }

    return static_cast<size_t>(maxColor + 1);
}

size_t GraphMetrics::getBridgesCountRandomized() const {
    if (m_randomBridgesCount) return *m_randomBridgesCount;

    size_t n = m_graph.getVerticesCount();
    if (n == 0) return 0;

    std::vector<uint64_t> vertexHashes(n, 0);
    std::vector<int> visited(n, 0);  // 0 - unvisited, 1 - visiting, 2 - visited
    std::vector<int> tin(n, -1);
    int timer = 0;
    size_t bridgeCount = 0;

    // 1. Генерируем случайные веса для всех циклов (back-edges)
    // Мы сделаем это прямо внутри DFS для экономии памяти
    for (size_t i = 0; i < n; ++i) {
        if (visited[i] == 0) {
            computeRandomBridges(i, DFS::NoParent, vertexHashes, visited, tin,
                                 timer, bridgeCount);
        }
    }

    m_randomBridgesCount = bridgeCount;
    return *m_randomBridgesCount;
}

void GraphMetrics::computeRandomBridges(size_t v, size_t p,
                                        std::vector<uint64_t>& vertexHashes,
                                        std::vector<int>& visited,
                                        std::vector<int>& tin, int& timer,
                                        size_t& bridgeCount) const {
    visited[v] = 1;  // В процессе
    tin[v] = timer++;

    // Инициализируем генератор (статический, чтобы не пересоздавать)
    static std::mt19937_64 rng(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dist;

    for (size_t to : m_graph.getNeighbors(v)) {
        if (to == p) continue;

        if (visited[to] == 1) {
            // Нашли обратное ребро (Back-edge) -> образовался цикл
            // Генерируем случайную метку для этого цикла
            uint64_t edgeHash = dist(rng);
            vertexHashes[v] ^= edgeHash;
            vertexHashes[to] ^= edgeHash;
        } else if (visited[to] == 0) {
            // Ребро дерева DFS

            computeRandomBridges(to, v, vertexHashes, visited, tin, timer,
                                 bridgeCount);

            // После возврата из рекурсии vertexHashes[to] содержит
            // XOR-сумму всех циклов, проходящих через ребро (v, to)
            if (vertexHashes[to] == 0) {
                bridgeCount++;
            }

            // Пробрасываем XOR-сумму вверх по дереву
            vertexHashes[v] ^= vertexHashes[to];
        }
    }

    visited[v] = 2;  // Полностью обработан
}
