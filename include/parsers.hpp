#ifndef PARSERS_HPP
#define PARSERS_HPP

#include <fstream>
#include <unordered_map>

#include "igraph.hpp"

enum class CurrentParser { EDGE_LIST, ADJACENCY_MATRIX, DIMACS, SNAP };

class IParser;

class ParserUtility {
   private:
    CurrentParser state;
    IParser* parser;

   public:
    ParserUtility(CurrentParser parser_code);
    ~ParserUtility();

    void reassignParser(CurrentParser new_parser_code);

    void parse(const std::string& filename, IGraph& g) const;
};

class IParser {
   public:
    virtual ~IParser() = default;

    /**
     * @brief Основной метод парсинга
     * @param filename Путь к файлу
     * @param g Ссылка на интерфейс графа
     */
    virtual void parse(const std::string& filename, IGraph& g) const = 0;
};

class EdgeListParser : public IParser {
    /* EdgeListParser
     * Графовый парсер из списка ребер
     */
   public:
    void parse(const std::string& filename, IGraph& g) const override;
};

class AdjacencyMatrixParser : public IParser {
    /* AdjacencyMatrixParser
     * Графовый парсер из матрицы смежности
     */
   public:
    void parse(const std::string& filename, IGraph& g) const override;

   private:
    size_t peekMatrixSize(std::ifstream& file) const;
};

class DIMACSParser : public IParser {
    /* DIMACSParser
     * Графовый парсер из формата DIMACS
     */
   public:
    void parse(const std::string& filename, IGraph& g) const override;
};

class SNAPParser : public IParser {
   private:
    mutable std::unordered_map<size_t, size_t> id_map;
    mutable size_t next_internal_id = 0;

   private:
    size_t getInternalId(size_t external_id) const;

   public:
    void parse(const std::string& filename, IGraph& g) const override;
};

#endif  // PARSERS_HPP
