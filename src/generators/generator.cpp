#include <algorithm>
#include <cstddef>
#include <graphodro4/core/igraph.hpp>
#include <graphodro4/generators/generator.hpp>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

#include "graphodro4/core/adjacency_list.hpp"
#include "graphodro4/debugging/_debugging.hpp"

std::random_device GraphFactory::rd;
std::mt19937 GraphFactory::gen(rd());

namespace {
enum class BaseModel { TREE, CYCLE };
}

size_t GraphFactory::complete(IGraph& g, size_t n) {
    g.reallocate(n);
    for (size_t i = 1; i < n; i++) {
        for (size_t j = i + 2; j < n; j++) {
            g.addEdge(i, j);
        }
    }
    return g.getEdgesCount();
}

size_t GraphFactory::completeBipartite(IGraph& g, size_t n, size_t m) {
    g.reallocate(n + m);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = n; j < n + m; j++) {
            g.addEdge(i, j);
        }
    }
    return g.getEdgesCount();
}

size_t GraphFactory::tree(IGraph& g, size_t n) {
    g.reallocate(n);

    if (n == 2) g.addEdge(0, 1);
    if (n < 3) return g.getEdgesCount();

    // Иначе n >= 3 и код Прюфера определен
    std::vector<size_t> code;
    GraphFactory::generatePruferCode(code, n);

    GraphFactory::decodePruferCode(g, code, n);

    return g.getEdgesCount();
}

size_t GraphFactory::star(IGraph& g, size_t n) {
    g.reallocate(n);

    std::uniform_int_distribution<size_t> dist(0, n - 1);
    size_t randIndex = dist(GraphFactory::gen);

    for (size_t i = 0; i < n; i++) {
        if (i == randIndex) continue;
        g.addEdge(i, randIndex);
    }

    return g.getEdgesCount();
}

size_t GraphFactory::cycle(IGraph& g, size_t n) {
    GraphFactory::path(g, n);
    if (n > 0) g.addEdge(0, n - 1);
    return g.getEdgesCount();
}

size_t GraphFactory::path(IGraph& g, size_t n) {
    g.reallocate(n);

    for (size_t i = 1; i < n; i++) {
        g.addEdge(i, i - 1);
    }

    return g.getEdgesCount();
}

size_t GraphFactory::wheel(IGraph& g, size_t n) {
    if (n < 3) return GraphFactory::cycle(g, n);

    // Колесо W(n) определено
    GraphFactory::cycle(g, n - 1);

    // Добавляем вершину и соединяем с центром
    if (n > 3) g.addVertices();
    for (size_t i = 0; i < n - 1; i++) {
        g.addEdge(i, n - 1);
    }

    return g.getEdgesCount();
}

size_t GraphFactory::random(IGraph& g, size_t n, const double p) {
    std::bernoulli_distribution dist(p);

    g.reallocate(n);
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (dist(GraphFactory::gen)) {
                // Добавляем ребро (i, j), i < j
                g.addEdge(i, j);
            }
        }
    }
    return g.getEdgesCount();
}

/**
 * @brief Генерирует случайный кубический граф.
 * Использует список свободных слотов для каждой вершины.
 */
size_t GraphFactory::cubic(IGraph& g, size_t n) {
    if (n % 2 != 0 || n < 4) {
        throw std::invalid_argument("N must be even and >= 4");
    }

    bool success = false;
    while (!success) {
        g.reallocate(n);
        // Создаем список всех доступных слотов: {0,0,0, 1,1,1, ...,
        // n-1,n-1,n-1}
        std::vector<size_t> slots;
        for (size_t i = 0; i < n; ++i) {
            for (int k = 0; k < 3; ++k) slots.push_back(i);
        }

        std::shuffle(slots.begin(), slots.end(), gen);

        success = true;
        for (size_t i = 0; i < slots.size(); i += 2) {
            size_t u = slots[i];
            size_t v = slots[i + 1];

            // Проверяем на петли и кратные ребра
            if (u == v || g.hasEdge(u, v)) {
                success = false;
                break;  // Тупик, начинаем заново (Restart)
            }
            g.addEdge(u, v);
        }
    }

    return g.getEdgesCount();
}

size_t GraphFactory::fixedComponents(IGraph& g, size_t n, size_t k,
                                     bool addNoise) {
    if (k > n)
        throw std::invalid_argument(
            "Graph of n vertices cannot be of k > n components!");

    std::vector<size_t> sizes = GraphFactory::splitVertices(n, k);

    std::cout << "Partition is ";
    debugging::print_vector(sizes);

    g.reallocate(n);

    size_t offset = 0;
    std::uniform_real_distribution<double> realDist(0, 1);
    double p_i;

    // Генерируем компоненту в диапазоне индексов [offset, offset + n_i - 1]
    for (size_t n_i : sizes) {
        std::uniform_int_distribution<size_t> discreteDist(0,
                                                           n_i * (n_i - 1) / 2);
        size_t max_i =
            discreteDist(gen);  // количество вершин, которые окажутся
                                // дополнительно соединенными

        p_i = addNoise ? realDist(GraphFactory::gen)
                       : 0;  // вероятность соединения пары вершин в
                             // компоненте i.
                             // Если 0, то дерево

        std::cout << "Populating graph with " << n_i << " vertices\n";
        GraphFactory::populateComponent(g, offset, n_i, max_i, p_i);
        offset += n_i;

        std::cout << g << "\n==============\n";
    }

    return g.getEdgesCount();
}

size_t GraphFactory::forest(IGraph& g, size_t n, size_t k) {
    return GraphFactory::fixedComponents(g, n, k, false);
}

/**
 * @brief Генерируем граф с n вершинами и ровно k мостами через
 * компоненты-циклы. Создаем k+1 случайных циклов и соединяем их k ребрами.
 */
size_t GraphFactory::fixedBridges(IGraph& g, size_t n, size_t k,
                                  const double p) {
    if (k >= n || n < 3 * (k + 1)) {
        // Для каждого цикла нужно минимум 3 вершины, чтобы не плодить лишние
        // мосты внутри
        throw std::invalid_argument("Недостаточно вершин для k+1 циклов");
    }

    g.reallocate(n);

    // Разбиваем вершины на k+1 группу
    std::vector<size_t> sizes = splitVertices(n, k + 1);
    size_t current_offset = 0;
    std::vector<size_t> component_roots;

    for (size_t i = 0; i < k + 1; ++i) {
        size_t m = sizes[i];
        // Запоминаем одну вершину из компоненты для последующего соединения
        component_roots.push_back(current_offset);

        // Создаем цикл внутри компоненты
        for (size_t j = 0; j < m; ++j) {
            size_t u = current_offset + j;
            size_t v = current_offset + (j + 1) % m;
            g.addEdge(u, v);
        }

        // Добавляем случайные ребра внутри цикла (шум), чтобы это не был просто
        // голый цикл Это не создаст новых мостов, но сделает граф "гуще"
        std::bernoulli_distribution dist(p);
        for (size_t u = current_offset; u < current_offset + m; ++u) {
            for (size_t v = u + 2; v < current_offset + m; ++v) {
                // Проверка: v - это не "следующая" вершина в цикле (j + 1 % m)
                // и не "предыдущая" (для последнего элемента)
                bool is_cycle_edge =
                    (v == u + 1) ||
                    (u == current_offset && v == current_offset + m - 1);

                if (!is_cycle_edge && dist(gen)) {
                    g.addEdge(u, v);
                }
            }
        }
        current_offset += m;
    }

    // Соединяем компоненты k ребрами, чтобы структура мета-графа была деревом
    // Используем простой путь между корнями компонент для создания k мостов
    for (size_t i = 0; i < k; ++i) {
        g.addEdge(component_roots[i], component_roots[i + 1]);
    }

    return g.getEdgesCount();
}

/**
 * @brief Генерируем граф с n вершинами и k точками сочленения.
 * Используем каскад циклов: вершины стыковки становятся точками сочленения.
 */
size_t GraphFactory::fixedArticulationPoints(IGraph& g, size_t n, size_t k,
                                             const double p) {
    if (k >= n - 1) throw std::invalid_argument("Too many articulation points");

    g.reallocate(n);
    if (n == 0) return 0;

    // Распределяем n-k "внутренних" вершин между k+1 блоками
    std::vector<size_t> sizes = splitVertices(n - k, k + 1);

    // Индекс вершины, которая станет первой точкой сочленения
    size_t current_free_v = k + 1;

    for (size_t i = 0; i < k + 1; ++i) {
        // Формируем блок i. Он включает:
        // 1. Точку сочленения i (вход)
        // 2. Точку сочленения i+1 (выход, если не последний блок)
        // 3. Свои уникальные вершины из массива sizes

        std::vector<size_t> block_nodes;
        block_nodes.push_back(i);  // Входная точка
        if (i < k)
            block_nodes.push_back(i + 1);  // Выходная точка (следующая AP)

        // Добавляем уникальные вершины в блок
        for (size_t j = 0; j < sizes[i]; ++j) {
            if (current_free_v < n) {
                block_nodes.push_back(current_free_v++);
            }
        }

        // Замыкаем блок в цикл для исключения мостов (2-связность)
        if (block_nodes.size() >= 3) {
            for (size_t j = 0; j < block_nodes.size(); ++j) {
                g.addEdge(block_nodes[j],
                          block_nodes[(j + 1) % block_nodes.size()]);
            }
            // Добавляем случайные хорды (шум) внутри блока
            std::bernoulli_distribution dist(p);
            for (size_t u = 0; u < block_nodes.size(); ++u) {
                for (size_t v = u + 2; v < block_nodes.size(); ++v) {
                    if (dist(gen) && (u != 0 || v != block_nodes.size() - 1)) {
                        g.addEdge(block_nodes[u], block_nodes[v]);
                    }
                }
            }
        } else if (block_nodes.size() == 2) {
            // Если вершин мало, создаем просто ребро (в этом случае AP будет
            // инцидентна мосту)
            g.addEdge(block_nodes[0], block_nodes[1]);
        }
    }

    return g.getEdgesCount();
}

/**
 * @brief Генерируем граф с n вершинами и k "2-мостами".
 * Добавляем случайные ребра только внутри блоков, чтобы сохранить разрезы.
 */
size_t GraphFactory::fixedTwoBridges(IGraph& g, size_t n, size_t k,
                                     const double p) {
    if (n < 2 * k + 2) throw std::invalid_argument("Too few vertices");

    g.reallocate(n);
    std::vector<size_t> sizes = splitVertices(n, k + 1);
    size_t current_offset = 0;
    std::vector<std::pair<size_t, size_t>> connectors;

    for (size_t i = 0; i < k + 1; ++i) {
        size_t m = sizes[i];
        size_t start = current_offset;

        // Добавляем базовый цикл (гарантируем 2-реберную связность блока)
        for (size_t j = 0; j < m; ++j) {
            g.addEdge(start + j, start + (j + 1) % m);
        }

        // --- ДОБАВЛЯЕМ ШУМ (ВНУТРИ БЛОКА) ---
        // Вероятность p=0.3 для появления хорд в цикле
        std::bernoulli_distribution dist(p);
        for (size_t u = start; u < start + m; ++u) {
            for (size_t v = u + 2; v < start + m; ++v) {
                // Избегаем дублирования ребер цикла
                if (u == start && v == start + m - 1) continue;
                if (dist(gen)) g.addEdge(u, v);
            }
        }

        connectors.push_back({start, start + m / 2});
        current_offset += m;
    }

    // Соединяем блоки парами ребер (эти пары и есть 2-мосты)
    for (size_t i = 0; i < k; ++i) {
        g.addEdge(connectors[i].first, connectors[i + 1].first);
        g.addEdge(connectors[i].second, connectors[i + 1].second);
    }

    return g.getEdgesCount();
}

/**
 * @brief Генерирует граф Халина на основе "звезды" или полного бинарного
 * дерева.
 */
size_t GraphFactory::halin(IGraph& g, size_t n) {
    if (n < 4)
        throw std::invalid_argument("Halin graph requires at least 4 vertices");

    g.reallocate(n);

    // Вариант 1: Колесо — это частный случай графа Халина
    if (n <= 6) {
        return wheel(g, n);
    }

    // Вариант 2: Халин на основе бамбука (для n > 6)
    // Создаем внутренний остов (вершины 0...n/2 - 1)
    size_t internal_count = n / 2;
    for (size_t i = 0; i < internal_count - 1; ++i) {
        g.addEdge(i, i + 1);
    }

    // Привязываем листья (остальные вершины) к внутренним узлам
    std::vector<size_t> leaves;
    for (size_t i = internal_count; i < n; ++i) {
        g.addEdge(i % internal_count, i);
        leaves.push_back(i);
    }

    // Замыкаем листья в цикл (внешняя грань)
    for (size_t i = 0; i < leaves.size(); ++i) {
        g.addEdge(leaves[i], leaves[(i + 1) % leaves.size()]);
    }

    return g.getEdgesCount();
}

// ========== Вспомогательные методы ============

void GraphFactory::generatePruferCode(std::vector<size_t>& code, size_t n) {
    std::uniform_int_distribution<size_t> randIndex(0, n - 1);

    code.resize(n - 2);
    for (size_t i = 0; i < code.size(); i++) {
        code[i] = randIndex(GraphFactory::gen);
    }
}

void GraphFactory::decodePruferCode(IGraph& g, std::vector<size_t>& code,
                                    size_t n) {
    std::vector<size_t> degrees(n, 1);
    for (size_t& x : code) {
        degrees[x]++;  // deg = количество упоминаний в коде + 1
    }

    std::set<size_t> leaves;

    // Ищем все листья
    for (size_t i = 0; i < n; i++) {
        if (degrees[i] == 1) leaves.insert(i);
    }

    for (size_t x : code) {
        // В начале leaves лежит текущий лист
        size_t leaf = *leaves.begin();
        leaves.erase(leaves.begin());

        g.addEdge(leaf, x);

        // Обновляем степень родителя
        degrees[x]--;
        if (degrees[x] == 1) {
            leaves.insert(x);
        }
    }

    // Гарантированно осталось две вершины!

    size_t i = *leaves.begin();
    leaves.erase(leaves.begin());
    size_t j = *leaves.begin();
    leaves.erase(leaves.begin());

    g.addEdge(i, j);
}

std::vector<size_t> GraphFactory::splitVertices(size_t n, size_t k) {
    if (k > n)
        throw std::invalid_argument(
            "Cannot partition n vertices into k > n components!");

    std::vector<size_t> sizes(k, 1);  // Каждой компоненте минимум 1 вершина
    size_t remaining = n - k;

    std::uniform_int_distribution<size_t> dist(0, k - 1);
    for (size_t i = 0; i < remaining; ++i) {
        sizes[dist(GraphFactory::gen)]++;
    }
    return sizes;
}

size_t GraphFactory::populateComponent(IGraph& g, size_t offset, size_t n_i,
                                       size_t max_i, const double p_i) {
    // Генерируем дерево (количество компонент гарантированно = 1)
    AdjacencyList tree;
    GraphFactory::tree(tree, n_i);

    std::cout << tree << "\n";

    std::bernoulli_distribution bernDist(p_i);
    std::uniform_int_distribution<size_t> unifDist(0, n_i - 1);

    size_t u, v;

    // Добавляем шум в список ребер
    for (size_t k = 0; k < max_i; k++) {
        if (bernDist(GraphFactory::gen)) {
            u = unifDist(GraphFactory::gen);
            v = unifDist(GraphFactory::gen);

            tree.addEdge(u, v);
        }
    }

    // Переносим компоненту в основной граф
    tree.forEachEdge(
        [&](size_t u, size_t v) { g.addEdge(offset + u, offset + v); });

    return tree.getEdgesCount();
}
