#include <algorithm>
#include <cassert>
#include <cstddef>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <stdexcept>

// ======== ГЕТТЕРЫ ==========

std::vector<size_t> AdjacencyMatrix::getNeighbors(size_t vertex) const {
    if (vertex >= p_n) throw std::out_of_range("Vertex out of range");

    auto start = p_adjMatrix.begin() + (vertex * p_n);
    return std::vector<size_t>(start, start + p_n);
};

size_t AdjacencyMatrix::getVerticesCount() const {
    return p_n;
};

size_t AdjacencyMatrix::getV() const {
    return p_n;
};

size_t AdjacencyMatrix::getEdgesCount() const {
    return p_edges;
};

size_t AdjacencyMatrix::getE() const {
    return p_edges;
};

bool AdjacencyMatrix::hasEdge(size_t from, size_t to) const {
    if (from >= p_n || to >= p_n)
        throw std::invalid_argument("Indices out of range!");

    if (p_adjMatrix[from * p_n + to]) return true;

    return false;
}

size_t AdjacencyMatrix::deg(size_t vertex) const {
    if (vertex >= p_n) throw std::out_of_range("Vertex index out of range");

    size_t degree = 0;
    size_t offset = vertex * p_n;
    for (size_t j = 0; j < p_n; ++j) {
        degree += p_adjMatrix[offset + j];
    }
    return degree;
}

const size_t* AdjacencyMatrix::operator[](size_t vertex) const {
    return &p_adjMatrix[vertex * p_n];
}

size_t* AdjacencyMatrix::operator[](size_t vertex) {
    return &p_adjMatrix[vertex * p_n];
}

const size_t* AdjacencyMatrix::at(size_t vertex) const {
    if (vertex >= p_adjMatrix.size()) {
        throw std::out_of_range("Vertex index out of range");
    }
    return &p_adjMatrix[vertex * p_n];
}

void AdjacencyMatrix::forEachEdge(
    std::function<void(size_t, size_t)> callback) const {
    for (size_t i = 0; i < p_n; i++) {
        for (size_t j = i; j < p_n; j++) {
            if (p_adjMatrix[i * p_n + j]) callback(i, j);
        }
    }
}

// ======== МОДИФИКАТОРЫ ========

void AdjacencyMatrix::reallocate(size_t n) {
    p_adjMatrix.assign(n * n, 0);
    p_n = n;
    p_edges = 0;
}

void AdjacencyMatrix::allocate(size_t n) {
    p_adjMatrix.assign(n * n, 0);

    p_n = n;
    p_edges = 0;
}

size_t AdjacencyMatrix::addEdge(size_t from, size_t to) {
    if (from >= p_n || to >= p_n) {
        addVertices(std::max(from, to) - p_n + 1);
    }

    p_adjMatrix[from * p_n + to] += 1;
    if (from != to) {
        // исключаем петли
        p_adjMatrix[to * p_n + from] += 1;
    }
    ++p_edges;
    return p_edges;
};

size_t AdjacencyMatrix::addVertices(size_t k) {
    if (k == 0) return p_n;

    size_t new_n = p_n + k;
    std::vector<size_t> new_storage(new_n * new_n, 0);

    // Копируем старые данные в новую (большую) матрицу
    for (size_t i = 0; i < p_n; ++i) {
        for (size_t j = 0; j < p_n; ++j) {
            new_storage[i * new_n + j] = p_adjMatrix[i * p_n + j];
        }
    }

    p_adjMatrix = std::move(new_storage);
    p_n = new_n;
    return p_n;
}

size_t AdjacencyMatrix::rmEdges(size_t from, size_t to, size_t occurrences) {
    if (from >= p_n || to >= p_n) return 0;

    size_t& valDirect = p_adjMatrix[from * p_n + to];
    size_t& valReverse = p_adjMatrix[to * p_n + from];

    // Проверка симметрии (теперь с корректным условием)
    if (valDirect != valReverse) {
        std::cerr << "Symmetry broken at [" << from << "][" << to << "]\n";
    }

    size_t toRemove = std::min({valDirect, valReverse, occurrences});

    valDirect -= toRemove;
    if (from != to) {
        // отдельно смотрим на НЕ петли
        valReverse -= toRemove;
    }

    p_edges -= toRemove;
    return toRemove;
}

// ======== ВСЯКОЕ =========

// Перегрузка std::cout << AdjacencyMatrix
void AdjacencyMatrix::_print(std::ostream& os) const {
    os << "Adjacency Matrix of G (|V|=" << p_n << ", |E|=" << p_edges << "):\n";
    if (p_n) {
        for (size_t i = 0; i < p_n; ++i) {
            os << "deg(" << i << ")=" << deg(i) << "\t";
            for (size_t j = 0; j < p_n; j++) {
                os << p_adjMatrix[i * p_n + j];
                if (j < p_n - 1) os << " ";
            };
            os << "\n";
        }
    } else {
        os << "Graph is EMPTY!";
    }
}
