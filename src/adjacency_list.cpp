#include "adjacency_list.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

std::vector<size_t> AdjacencyList::getNeighbors(size_t vertex) const {
    return p_adjList.at(vertex);
};

size_t AdjacencyList::getVertexCount() const {
    return p_n;
};

size_t AdjacencyList::getN() const {
    return p_n;
};

size_t AdjacencyList::deg(size_t vertex) const {
    if (vertex < this->p_adjList.size()) return this->p_adjList[vertex].size();

    throw std::out_of_range("Vertex is not in graph!");
}

size_t AdjacencyList::addEdge(size_t from, size_t to) {
    // Проверка на трезвость
    size_t max = from > to ? from : to;
    if (max + 1 > p_n) this->addVertex(max - p_n + 1);

    p_adjList[from].emplace_back(to);
    if (from != to) {  // исключаем петли
        p_adjList[to].emplace_back(from);
    }
    ++p_edges;

    return p_edges;
};

size_t AdjacencyList::addVertex(uint8_t k) {
    for (uint8_t i = 0; i < k; i++) {
        p_adjList.emplace_back();
        p_n++;
    }

    return p_n;
}

static size_t removeFirstN(std::vector<size_t>& vec, size_t target,
                           size_t occurrences);

size_t AdjacencyList::rmEdges(size_t from, size_t to, size_t occurrences) {
    size_t fromToRemovals =
        removeFirstN(this->p_adjList[from], to, occurrences);

    size_t toFromRemovals = fromToRemovals;
    if (from != to)
        toFromRemovals = removeFirstN(this->p_adjList[to], from, occurrences);

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

// Перегрузка std::cout << AdjacencyList
void AdjacencyList::_print(std::ostream& os) const {
    os << "Adjacency List of G (|V|=" << this->p_n << ", |E|=" << this->p_edges
       << "):\n";
    for (size_t i = 0; i < this->p_adjList.size(); ++i) {
        os << i << " -> { ";
        for (size_t neighbor : this->p_adjList[i]) {
            os << neighbor << " ";
        }
        os << "}" << (i == this->p_adjList.size() - 1 ? "." : ";\n");
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
