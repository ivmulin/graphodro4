#ifndef DFS_HPP
#define DFS_HPP

#include "igraph.hpp"

class DFS {
   private:
    const IUnweightedGraph& m_graph;

   public:
    DFS(const IUnweightedGraph& graph);

    void dfs(size_t start);

   private:
    void dfsUtil(const IUnweightedGraph& graph, size_t vertex,
                 std::vector<bool>& visited);
};

#endif  // DFS_HPP
