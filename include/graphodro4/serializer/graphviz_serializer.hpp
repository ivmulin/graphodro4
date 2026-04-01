#pragma once

#include <algorithm>
#include <fstream>
#include <graphodro4/core/igraph.hpp>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Структура для хранения настроек визуализации ребра
 */
struct EdgeStyle {
    std::string color = "black";
    std::string style = "solid";  // solid, dashed, dotted
    double penwidth = 1.0;

    std::string toAttrString() const {
        std::stringstream ss;
        ss << "color=\"" << color << "\", style=" << style
           << ", penwidth=" << penwidth;
        return ss.str();
    }
};

/**
 * @brief Структура для хранения настроек визуализации вершины
 */
struct VertexStyle {
    std::string color = "black";      // Цвет границы
    std::string fillcolor = "white";  // Цвет заливки
    std::string style = "filled";     // filled, solid
    std::string shape = "circle";

    std::string toAttrString() const {
        std::stringstream ss;
        ss << "color=\"" << color << "\", fillcolor=\"" << fillcolor
           << "\", style=\"" << style << "\", shape=" << shape;
        return ss.str();
    }
};

/**
 * @brief Класс для сериализации графа в формат GraphViz (.dot)
 */
class GraphVizSerializer {
   public:
    explicit GraphVizSerializer(const IGraph& graph);

    /**
     * @brief Сохранить граф в файл .dot
     * @param filename Имя выходного файла
     * @param title Заголовок графа
     */
    void saveToFile(const std::string& filename,
                    const std::string& title = "Graph") const;

    /**
     * @brief Сериализовать граф в строку
     */
    std::string serialize(const std::string& title = "Graph") const;

    /**
     * @brief Полная сериализация с настройками
     */
    std::string serializeFull(
        const std::string& title = "Graph",
        const std::map<size_t, VertexStyle>& vertexStyles = {},
        const std::map<std::pair<size_t, size_t>, EdgeStyle>& edgeStyles = {},
        const std::vector<std::vector<size_t>>& clusters = {},
        const std::vector<std::pair<size_t, size_t>>& spanningTreeEdges = {},
        const std::vector<size_t>& cycleVertices = {}) const;

    /**
     * @brief Полная сериализация в поток
     */
    void serializeFull(
        std::ostream& os, const std::string& title = "Graph",
        const std::map<size_t, VertexStyle>& vertexStyles = {},
        const std::map<std::pair<size_t, size_t>, EdgeStyle>& edgeStyles = {},
        const std::vector<std::vector<size_t>>& clusters = {},
        const std::vector<std::pair<size_t, size_t>>& spanningTreeEdges = {},
        const std::vector<size_t>& cycleVertices = {}) const;

    /**
     * @brief Проверка валидности DOT формата (базовая)
     */
    bool isValidDotFormat(const std::string& dotContent) const;

   private:
    const IGraph& graph_;

    /**
     * @brief Нормализация ребра (мин, макс)
     */
    std::pair<size_t, size_t> normalizeEdge(size_t u, size_t v) const;
};
