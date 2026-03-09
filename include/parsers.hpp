#ifndef PARSERS_HPP
#define PARSERS_HPP

#include <vector>

#include "igraph.hpp"

class IParser {
    /* IParser
     * Базовый интерфейс для парсера графа
     *
     * Требуется для реализации интерфейсов
     */
   public:
    virtual ~IParser() = default;
    virtual void parse(const std::string& filename, IGraph& g) const = 0;
};

class EdgeListParser : public IParser {
    /* EdgeListParser
     * Графовый парсер из списка ребер
     */
   public:
    void parse(const std::string& filename, IGraph& g) const override;
    void parseFromEdgeList(const std::vector<std::pair<int, int>>& edgeList,
                           IGraph& g);
};

// class AdjacencyListParser : public IParser {
//     /* AdjacencyListParser
//      * Графовый парсер из списка смежности
//      */
//    public:
//     void parse(const std::string& filename, IGraph& g) const override;
//     void parseFromAdjList(const std::vector<std::vector<int>>& adjList,
//                           IGraph& g);
// };

#endif  // PARSERS_HPP
