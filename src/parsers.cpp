#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "igraph.hpp"
#include "parsers.hpp"

ParserUtility::ParserUtility(CurrentParser parser_code) : state(parser_code) {
    switch (parser_code) {
        case CurrentParser::EDGE_LIST:
            parser = new EdgeListParser();
            break;
        case CurrentParser::ADJACENCY_MATRIX:
            parser = new AdjacencyMatrixParser();
            break;
        case CurrentParser::DIMACS:
            parser = new DIMACSParser();
            break;
        case CurrentParser::SNAP:
            parser = new SNAPParser();
            break;
    }
}

void ParserUtility::reassignParser(CurrentParser new_parser_code) {
    delete parser;
    state = new_parser_code;
    switch (new_parser_code) {
        case CurrentParser::EDGE_LIST:
            parser = new EdgeListParser();
            break;
        case CurrentParser::ADJACENCY_MATRIX:
            parser = new AdjacencyMatrixParser();
            break;
        case CurrentParser::DIMACS:
            parser = new DIMACSParser();
            break;
        case CurrentParser::SNAP:
            parser = new SNAPParser();
            break;
    }
}

void ParserUtility::parse(const std::string& filename, IGraph& g) const {
    parser->parse(filename, g);
}

ParserUtility::~ParserUtility() {
    delete parser;
}

// ============= EdgeListParser ============= //

void EdgeListParser::parse(const std::string& filename, IGraph& g) const {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Cannot open edge list file");

    std::string line;
    while (std::getline(file, line)) {
        // Пропускаем пустые строки
        if (line.empty()) continue;

        // Игнорируем комментарии
        if (line[0] == '#' || line[0] == '%') continue;

        std::stringstream ss(line);
        size_t u, v;

        // Читаем пару вершин.
        if (ss >> u >> v) {
            g.addEdge(u, v);
        }
    }
}

// ============= AdjacencyMatrixParser ============= //

void AdjacencyMatrixParser::parse(const std::string& filename,
                                  IGraph& g) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("RawParser: Cannot open " + filename);
    }

    // Определяем размерность n.
    size_t n = this->peekMatrixSize(file);
    if (n == 0) return;

    // Выделяем память под граф
    g.allocate(n);

    // Читаем данные
    size_t value;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (!(file >> value)) {
                throw std::runtime_error("RawParser: Unexpected EOF at " +
                                         std::to_string(i) + ":" +
                                         std::to_string(j));
            }

            // Оптимизация для неориентированных графов:
            // добавляем ребро только один раз (для верхнего треугольника)
            // или проверяем наличие ребра (value > 0).
            if (value > 0 && i <= j) {
                // Используем addMultipleEdges, если в ячейке может быть > 1
                g.addMultipleEdges(i, j, value);
            }
        }
    }
}

size_t AdjacencyMatrixParser::peekMatrixSize(std::ifstream& file) const {
    std::string firstLine;
    if (!std::getline(file, firstLine)) {
        return 0;
    }

    // Если строка пустая (например, комментарий или пробел),
    // пропускаем её до первой значимой строки
    while (firstLine.empty() ||
           firstLine.find_first_not_of(" \t\r\n") == std::string::npos) {
        if (!std::getline(file, firstLine)) return 0;
    }

    std::stringstream ss(firstLine);
    size_t n = 0;
    size_t dummy;

    // Считаем количество чисел в первой строке
    while (ss >> dummy) {
        n++;
    }

    // ВАЖНО: После getline и подсчета нужно вернуть указатель
    // чтения в начало файла, чтобы parse() мог прочитать всё заново
    file.clear();                  // Сбрасываем флаги (например, EOF)
    file.seekg(0, std::ios::beg);  // Возвращаемся в начало

    return n;
}

// ============= DIMACSParser ============= //

void DIMACSParser::parse(const std::string& filename, IGraph& g) const {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Open failed");

    std::string line;
    while (std::getline(file, line)) {
        // Комментарий или пустая строка - пропускаем
        if (line.empty() || line[0] == 'c') continue;

        // Определяем команду
        std::stringstream ss(line);
        char command;
        ss >> command;

        if (command == 'p') {
            // p - объявление графа
            std::string dummy;
            size_t vertices, edges;
            ss >> dummy >> vertices >> edges;
            g.allocate(vertices);
        } else if (command == 'e') {
            // e - ребро
            size_t u, v;
            ss >> u >> v;
            g.addEdge(u - 1, v - 1);
        } else {
            continue;
        }
    }
}

// ============= SNAPParser ============= //

size_t SNAPParser::getInternalId(size_t external_id) const {
    if (id_map.find(external_id) == id_map.end()) {
        id_map[external_id] = next_internal_id++;
    }
    return id_map[external_id];
}

void SNAPParser::parse(const std::string& filename, IGraph& g) const {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Cannot open edge list file");

    id_map.clear();
    next_internal_id = 0;

    std::string line;
    while (std::getline(file, line)) {
        // Пропуск пустых строк
        if (line.empty()) continue;

        // Игнорирование комментариев (то самое "просто")
        if (line[0] == '#' || line[0] == '%') continue;

        std::stringstream ss(line);
        size_t u_real, v_real;
        size_t u_int, v_int;

        // Чтение пары ID
        if (ss >> u_real >> v_real) {
            u_int = getInternalId(u_real);
            v_int = getInternalId(v_real);

            g.addEdge(u_int, v_int);
        }
    }
}
