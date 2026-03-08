#include "adjacency_list.hpp"

#include <cstddef>
#include <cstdint>

std::vector<int> AdjacencyList::getNeighbors(int vertex) const {
    return p_adjList.at(vertex);
};

size_t AdjacencyList::getVertexCount() const {
    return p_n;
};

size_t AdjacencyList::getN() const {
    return p_n;
};

int AdjacencyList::addEdge(int from, int to) {
    // Проверка на трезвость
    size_t max = from > to ? from : to;
    if (max + 1 > p_n) this->addVertex(max - p_n + 2);

    p_adjList[from].emplace_back(to);
    ++p_edges;
    if (from != to) {  // исключаем петли
        p_adjList[to].emplace_back(from);
        ++p_edges;
    }

    return p_edges;
};

int AdjacencyList::addVertex(uint8_t k) {
    for (uint8_t i = 0; i < k; i++) {
        p_adjList.emplace_back();
        p_n++;
    }

    return p_n;
}

// Перегрузка std::cout << AdjacencyList
void AdjacencyList::_print(std::ostream& os) const {
    os << "Список смежности графа (|V|=" << this->p_n
       << ", |E|=" << this->p_edges << "):\n";
    for (size_t i = 0; i < this->p_adjList.size(); ++i) {
        os << i << " -> { ";
        for (int neighbor : this->p_adjList[i]) {
            os << neighbor << " ";
        }
        os << "}" << (i == this->p_adjList.size() - 1 ? "." : ";\n");
    }
}
