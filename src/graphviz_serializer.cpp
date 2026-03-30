#include "graphviz_serializer.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

GraphVizSerializer::GraphVizSerializer(const IGraph& graph) 
    : graph_(graph) {}

std::pair<size_t, size_t> GraphVizSerializer::normalizeEdge(
    size_t u, size_t v) const {
    return (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);
}

void GraphVizSerializer::saveToFile(const std::string& filename, 
                                     const std::string& title) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    serializeFull(file, title);
    file.close();
}

std::string GraphVizSerializer::serialize(const std::string& title) const {
    std::stringstream ss;
    serializeFull(ss, title);
    return ss.str();
}

std::string GraphVizSerializer::serializeFull(
    const std::string& title,
    const std::map<size_t, VertexStyle>& vertexStyles,
    const std::map<std::pair<size_t, size_t>, EdgeStyle>& edgeStyles,
    const std::vector<std::vector<size_t>>& clusters,
    const std::vector<std::pair<size_t, size_t>>& spanningTreeEdges,
    const std::vector<size_t>& cycleVertices) const {
    
    std::stringstream ss;
    serializeFull(ss, title, vertexStyles, edgeStyles, clusters, 
                  spanningTreeEdges, cycleVertices);
    return ss.str();
}

void GraphVizSerializer::serializeFull(
    std::ostream& os,
    const std::string& title,
    const std::map<size_t, VertexStyle>& vertexStyles,
    const std::map<std::pair<size_t, size_t>, EdgeStyle>& edgeStyles,
    const std::vector<std::vector<size_t>>& clusters,
    const std::vector<std::pair<size_t, size_t>>& spanningTreeEdges,
    const std::vector<size_t>& cycleVertices) const {
    
    os << "graph G {\n";
    os << "  label=\"" << title << "\";\n";
    os << "  node [fontname=\"Helvetica\", style=filled, fillcolor=white];\n";
    os << "  edge [fontname=\"Helvetica\", color=black];\n";

    std::set<size_t> verticesInClusters;
    
    for (size_t i = 0; i < clusters.size(); ++i) {
        os << "  subgraph cluster_" << i << " {\n";
        os << "    label=\"Component " << i << "\";\n";
        os << "    style=dashed;\n";
        os << "    color=gray;\n";
        
        for (size_t v : clusters[i]) {
            verticesInClusters.insert(v);
            os << "    " << v;
            
            auto it = vertexStyles.find(v);
            if (it != vertexStyles.end()) {
                os << " [" << it->second.toAttrString() << "]";
            }
            os << ";\n";
        }
        os << "  }\n";
    }

    for (size_t v = 0; v < graph_.getV(); ++v) {
        if (verticesInClusters.find(v) == verticesInClusters.end()) {
            os << "  " << v;
            auto it = vertexStyles.find(v);
            if (it != vertexStyles.end()) {
                os << " [" << it->second.toAttrString() << "]";
            }
            os << ";\n";
        }
    }

    std::set<std::pair<size_t, size_t>> treeEdgeSet;
    for (const auto& edge : spanningTreeEdges) {
        treeEdgeSet.insert(normalizeEdge(edge.first, edge.second));
    }

    std::set<std::pair<size_t, size_t>> cycleEdgeSet;
    if (!cycleVertices.empty()) {
        for (size_t i = 0; i < cycleVertices.size(); ++i) {
            size_t u = cycleVertices[i];
            size_t v = cycleVertices[(i + 1) % cycleVertices.size()];
            cycleEdgeSet.insert(normalizeEdge(u, v));
        }
    }

    for (size_t u = 0; u < graph_.getV(); ++u) {
        const auto& neighbors = graph_.getNeighbors(u);
        for (size_t v : neighbors) {
            if (u >= v) continue;

            std::string attrs;
            bool isTreeEdge = (treeEdgeSet.find({u, v}) != treeEdgeSet.end());
            bool isCycleEdge = (cycleEdgeSet.find({u, v}) != cycleEdgeSet.end());

            if (isCycleEdge) {
                attrs = "color=\"red\", penwidth=3.0, style=bold";
            } else if (isTreeEdge) {
                attrs = "color=\"green\", penwidth=2.0, style=solid";
            } else {
                auto key = std::make_pair(u, v);
                auto it = edgeStyles.find(key);
                if (it != edgeStyles.end()) {
                    attrs = it->second.toAttrString();
                }
            }

            os << "  " << u << " -- " << v;
            if (!attrs.empty()) {
                os << " [" << attrs << "]";
            }
            os << ";\n";
        }
    }

    os << "}\n";
}

bool GraphVizSerializer::isValidDotFormat(const std::string& dotContent) const {
    if (dotContent.find("graph G {") == std::string::npos) return false;
    if (dotContent.find("}") == std::string::npos) return false;
    
    int braceCount = 0;
    for (char c : dotContent) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        if (braceCount < 0) return false;
    }
    return braceCount == 0;
}