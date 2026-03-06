#include <iostream>

#include "igraph.hpp"

class Graph : public IUnweightedGraph {
   private:
    int n = 0;

   public:
    std::vector<int> getNeighbors(int vertex) const override {
        return {};
    };

    int getVertexCount() const override {
        return n;
    };

    int getN() const override {
        return n;
    };

    void addEdge(int from, int to) override {
        std::cout << "to be defined...\n";
    };
};

int main() {
    std::cout << "Hello!\n";
    Graph g;
    g.addEdge(0, 1);
    std::cout << "Fin!\n";
    return 0;
}
