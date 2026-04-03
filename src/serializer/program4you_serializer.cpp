// program4you_serializer.cpp
#include <graphodro4/serializer/program4you_serializer.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <set>

Program4YouSerializer::Program4YouSerializer(const IGraph& graph) 
    : graph_(graph) {
}

void Program4YouSerializer::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << serialize();
    file.close();
}

std::string Program4YouSerializer::serialize() const {
    std::stringstream ss;
    
    // Первая строка: количество вершин и ребер
    ss << graph_.getV() << " " << graph_.getE() << "\n";
    
    // Записываем все ребра (каждое ребро только один раз)
    std::set<std::pair<size_t, size_t>> written_edges;
    
    for (size_t u = 0; u < graph_.getV(); ++u) {
        const auto& neighbors = graph_.getNeighbors(u);
        for (size_t v : neighbors) {
            // Нормализуем ребро (меньший индекс первым)
            size_t from = std::min(u, v);
            size_t to = std::max(u, v);
            
            // Проверяем, не записано ли уже это ребро
            if (written_edges.find({from, to}) == written_edges.end()) {
                ss << from << " " << to << "\n";
                written_edges.insert({from, to});
            }
        }
    }
    
    return ss.str();
}

bool Program4YouSerializer::isValidFormat(const std::string& content) const {
    std::istringstream iss(content);
    std::string line;
    
    // Проверяем первую строку (должна содержать два числа)
    if (!std::getline(iss, line)) {
        return false;
    }
    
    std::istringstream header(line);
    size_t n, m;
    if (!(header >> n >> m)) {
        return false;
    }
    
    // Проверяем количество строк с ребрами
    size_t edge_count = 0;
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        
        std::istringstream edge_line(line);
        size_t u, v;
        if (!(edge_line >> u >> v)) {
            return false;
        }
        edge_count++;
    }
    
    return edge_count == m;
}

void Program4YouSerializer::loadFromFile(const std::string& filename, IGraph& graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    parse(buffer.str(), graph);
}

void Program4YouSerializer::parse(const std::string& content, IGraph& graph) {
    std::istringstream iss(content);
    std::string line;
    
    // Читаем первую строку (количество вершин и ребер)
    if (!std::getline(iss, line)) {
        throw std::runtime_error("Empty file or invalid format");
    }
    
    std::istringstream header(line);
    size_t n, m;
    if (!(header >> n >> m)) {
        throw std::runtime_error("Invalid header format");
    }
    
    // Выделяем память под граф
    graph.allocate(n);
    
    // Читаем ребра
    size_t edge_count = 0;
    while (std::getline(iss, line) && edge_count < m) {
        if (line.empty()) continue;
        
        std::istringstream edge_line(line);
        size_t u, v;
        if (edge_line >> u >> v) {
            graph.addEdge(u, v);
            edge_count++;
        }
    }
    
    if (edge_count != m) {
        std::cerr << "Warning: Expected " << m << " edges, but read " << edge_count << std::endl;
    }
}