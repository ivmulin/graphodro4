#include "adjacency_list.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
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

size_t AdjacencyList::deg(size_t vertex) const {
    if (vertex < p_adjList.size()) return p_adjList[vertex].size();

    throw std::out_of_range("Vertex is not in graph!");
}

const std::vector<size_t>& AdjacencyList::operator[](size_t vertex) const {
    // В низкоуровневых операторах [] обычно не делают проверок для скорости,
    // полагаясь на вызывающего (аналогично std::vector::operator[]).
    return p_adjList[vertex];
}

const std::vector<size_t>& AdjacencyList::at(size_t vertex) const {
    if (vertex >= p_adjList.size()) {
        throw std::out_of_range("Vertex index out of range");
    }
    return p_adjList.at(vertex);
}

// ======== МОДИФИКАТОРЫ ========

size_t AdjacencyList::addEdge(size_t from, size_t to) {
    // Проверка на трезвость
    size_t max = std::max({p_n, from, to});
    if (max + 1 > p_n) addVertices(max - p_n + 1);

    p_adjList[from].emplace_back(to);
    if (from != to) {  // исключаем петли
        p_adjList[to].emplace_back(from);
    }
    ++p_edges;

    return p_edges;
};

size_t AdjacencyList::addVertices(uint8_t k) {
    for (uint8_t i = 0; i < k; i++) {
        p_adjList.emplace_back();
        p_n++;
    }

    return p_n;
}

static size_t removeFirstN(std::vector<size_t>& vec, size_t target,
                           size_t occurrences);

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

// ======== ВСЯКОЕ =========

// Перегрузка std::cout << AdjacencyList
void AdjacencyList::_print(std::ostream& os) const {
    os << "Adjacency List of G (|V|=" << p_n << ", |E|=" << p_edges << "):\n";
    for (size_t i = 0; i < p_adjList.size(); ++i) {
        os << i << " -> { ";
        for (size_t neighbor : p_adjList[i]) {
            os << neighbor << " ";
        }
        os << "}" << (i == p_adjList.size() - 1 ? "." : ";\n");
    }
}

static size_t removeFirstN(std::vector<size_t>& vec, size_t target,
                           size_t occurrences) {
    size_t count = 0;
    for (size_t i = 0; i < vec.size() && count < occurrences;) {
        if (vec[i] == target) {
            // Swap with the last element and remove it
            vec[i] = std::move(vec.back());
            vec.pop_back();
            count++;
            // Note: Don't increment 'i' here because the new
            // vec[i] (the former back element) needs to be checked
        } else {
            i++;
        }
    }
    return count;
}
