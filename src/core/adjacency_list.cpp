#include <algorithm>
#include <cassert>
#include <cstddef>
#include <graphodro4/core/adjacency_list.hpp>
#include <stdexcept>

// ======== ГЕТТЕРЫ ==========

std::vector<size_t> AdjacencyList::getNeighbors(size_t vertex) const {
    return p_adjList.at(vertex);
};

size_t AdjacencyList::getVerticesCount() const {
    return p_n;
};

size_t AdjacencyList::getV() const {
    return p_n;
};

size_t AdjacencyList::getEdgesCount() const {
    return p_edges;
};

size_t AdjacencyList::getE() const {
    return p_edges;
};

bool AdjacencyList::hasEdge(size_t from, size_t to) const {
    if (from >= p_n || to >= p_n)
        throw std::out_of_range("Indices out of range!");

    if (std::find(p_adjList[from].begin(), p_adjList[from].end(), to) !=
        p_adjList[from].end())
        return true;

    return false;
}

size_t AdjacencyList::deg(size_t vertex) const {
    if (vertex < p_adjList.size()) return p_adjList[vertex].size();

    throw std::out_of_range("Vertex is not in graph!");
}

const std::vector<size_t>& AdjacencyList::operator[](size_t vertex) const {
    // В низкоуровневых операторах [] обычно не делают проверок для
    // скорости, полагаясь на вызывающего (аналогично
    // std::vector::operator[]).
    return p_adjList[vertex];
}

const std::vector<size_t>& AdjacencyList::at(size_t vertex) const {
    if (vertex >= p_adjList.size()) {
        throw std::out_of_range("Vertex index out of range");
    }
    return p_adjList.at(vertex);
}

// ======== МОДИФИКАТОРЫ ========

void AdjacencyList::reallocate(size_t n) {
    if (n == p_n) return;

    size_t old_n = p_n;

    if (n < old_n) {
        size_t totalRemoved = 0;
        // 1. Считаем ребра, которые выходят из остающихся вершин во внешние
        for (size_t i = 0; i < n; ++i) {
            auto& neighbors = p_adjList[i];
            auto it = std::remove_if(neighbors.begin(), neighbors.end(),
                                     [n](size_t v) { return v >= n; });
            totalRemoved += std::distance(it, neighbors.end());
            neighbors.erase(it, neighbors.end());
        }

        // 2. Считаем все ребра в удаляемых списках (i >= n)
        for (size_t i = n; i < old_n; ++i) {
            totalRemoved += p_adjList[i].size();
        }

        // Каждое ребро было посчитано дважды (u->v и v->u)
        p_edges -= (totalRemoved / 2);
    }

    p_adjList.resize(n);
    p_n = n;
}

void AdjacencyList::allocate(size_t n) {
    p_adjList.assign(n, std::vector<size_t>());

    p_n = n;
    p_edges = 0;
}

size_t AdjacencyList::addEdge(size_t from, size_t to) {
    // Проверка на трезвость
    size_t maxIndex = std::max(from, to);
    if (maxIndex >= p_n) addVertices(maxIndex - p_n + 1);

    p_adjList[from].emplace_back(to);
    if (from != to) {  // исключаем петли
        p_adjList[to].emplace_back(from);
    }
    ++p_edges;

    return p_edges;
};

size_t AdjacencyList::addVertices(size_t k) {
    for (size_t i = 0; i < k; i++) {
        p_adjList.emplace_back();
        p_n++;
    }

    return p_n;
}

size_t AdjacencyList::rmEdges(size_t from, size_t to, size_t occurrences) {
    size_t fromToRemovals = removeFirstN(p_adjList[from], to, occurrences);

    size_t toFromRemovals = fromToRemovals;
    if (from != to)
        toFromRemovals = removeFirstN(p_adjList[to], from, occurrences);

    assert(fromToRemovals == toFromRemovals &&
           "Symmetry broken in undirected graph!");

    if (fromToRemovals == toFromRemovals) {
        // исключаем петли
        p_edges -= fromToRemovals;
    } else {
        // TODO: Добавить проверку графа на неориентированность

        // ошибка! Нарушена неориентированность графа
        throw std::logic_error("Graph integrity violation");
    }

    return fromToRemovals;
}

void AdjacencyList::forEachEdge(
    std::function<void(size_t, size_t)> callback) const {
    for (size_t i = 0; i < p_n; i++) {
        for (size_t j : p_adjList[i]) {
            if (i <= j) callback(i, j);
        }
    }
}

// ======== ВСЯКОЕ =========

// Перегрузка std::cout << AdjacencyList
void AdjacencyList::_print(std::ostream& os) const {
    os << "Adjacency List of G (|V|=" << p_n << ", |E|=" << p_edges << "):\n";
    if (p_n) {
        for (size_t i = 0; i < p_n; ++i) {
            os << "deg(" << i << ")=" << deg(i) << "\t";
            os << "G[" << i << "]={ ";
            for (size_t neighbor : p_adjList[i]) {
                os << neighbor << " ";
            }
            os << "}" << (i == p_adjList.size() - 1 ? "." : ";\n");
        }
    } else {
        os << "Graph is EMPTY!";
    }
}

size_t AdjacencyList::removeFirstN(std::vector<size_t>& vector, size_t target,
                                   size_t occurrences) {
    size_t count = 0;
    for (size_t i = 0; i < vector.size() && count < occurrences;) {
        if (vector[i] == target) {
            // Swap with the last element and remove it
            vector[i] = std::move(vector.back());
            vector.pop_back();
            count++;
            // Note: Don't increment 'i' here because the new
            // vec[i] (the former back element) needs to be checked
        } else {
            i++;
        }
    }
    return count;
}
