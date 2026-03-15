#ifndef PARSERS_HPP
#define PARSERS_HPP

#include <fstream>

#include "igraph.hpp"

class IParser {
    /* IParser
     * Базовый интерфейс для парсера графа
     *
     * Требуется для реализации интерфейсов
     */
   public:
    virtual ~IParser() = default;
    virtual void parse(const std::string& filename,
                       IUnweightedGraph& g) const = 0;
    // virtual void parseFromEdgeList(
    //     const std::vector<std::pair<int, int>>& edgeList, IGraph& g) const =
    //     0;
    // virtual void parseFromAdjMatrix(const std::vector<std::vector<int>>&
    // adjList,
    //                         IGraph& g) const override;
};

class EdgeListParser : public IParser {
    /* EdgeListParser
     * Графовый парсер из списка ребер
     */
   public:
    void parse(const std::string& filename, IUnweightedGraph& g) const override;
    // void parseFromEdgeList(const std::vector<std::pair<int, int>>& edgeList,
    //                        IGraph& g) const override;
};

class AdjMatrixParser : public IParser {
    /* AdjacencyListParser
     * Графовый парсер из матрицы смежности
     */
   public:
    void parse(const std::string& filename, IUnweightedGraph& g) const override;
    // void parseFromAdjMatrix(const std::vector<std::vector<int>>& adjList,
    //                         IGraph& g) const override;
   private:
    size_t peekMatrixSize(std::ifstream& file) const;
};

#endif  // PARSERS_HPP
