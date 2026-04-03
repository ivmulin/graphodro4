// ВНИМАНИЕ! БОЛЬШАЯ ЗАПРОМПЧЕННАЯ ФИГНЯ, НОРМАЛЬНАЯ БИБЛИОТЕКА НЕ ХОТЕЛА СТАВИТСЯ НА ВИНДУ (СЛАВА ЛИНУКСУ)
// ПОЭТОМУ ОНО ВЫГЛЯДИТ ТАК ПЛОХО, ПРИЯТНОГО ПРОСМОТРА!
#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <iomanip>
#include <limits>
#include <graphodro4/core/adjacency_list.hpp>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <graphodro4/generators/generator.hpp>
#include <graphodro4/parsers/parsers.hpp>
#include <graphodro4/serializer/graphviz_serializer.hpp>
#include <graphodro4/serializer/program4you_serializer.hpp>
#include <graphodro4/algorithms/metrics.hpp>
#include <graphodro4/algorithms/dfs.hpp>

#ifdef _WIN32
#include <windows.h>
#endif



class ConsoleUI {
private:
    std::unique_ptr<IUnweightedGraph> current_graph;
    ParserUtility parser{CurrentParser::EDGE_LIST};
    
    void clearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    std::string getUserInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    
    size_t getNumber(const std::string& prompt, size_t min = 0, size_t max = SIZE_MAX) {
        while (true) {
            std::cout << prompt;
            std::string input;
            std::getline(std::cin, input);
            try {
                size_t value = std::stoul(input);
                if (value >= min && value <= max) {
                    return value;
                }
                std::cout << "Value must be between " << min << " and " << max << "\n";
            } catch (...) {
                std::cout << "Invalid number! Try again.\n";
            }
        }
    }
    
    double getDouble(const std::string& prompt, double min = 0.0, double max = 1.0) {
        while (true) {
            std::cout << prompt;
            std::string input;
            std::getline(std::cin, input);
            try {
                double value = std::stod(input);
                if (value >= min && value <= max) {
                    return value;
                }
                std::cout << "Value must be between " << min << " and " << max << "\n";
            } catch (...) {
                std::cout << "Invalid number! Try again.\n";
            }
        }
    }
    
    bool selectBackend() {
        std::cout << "\n=== Select Graph Backend ===\n";
        std::cout << "1. Adjacency List\n";
        std::cout << "2. Adjacency Matrix\n";
        std::cout << "Choice: ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            current_graph = std::make_unique<AdjacencyList>();
            std::cout << "Selected: Adjacency List\n";
            return true;
        } else if (choice == "2") {
            current_graph = std::make_unique<AdjacencyMatrix>();
            std::cout << "Selected: Adjacency Matrix\n";
            return true;
        }
        
        std::cout << "Invalid choice!\n";
        return false;
    }
    
    void showGraphInfo() {
        if (!current_graph) {
            std::cout << "No active graph!\n";
            return;
        }
        
        std::cout << "\n=== Graph Information ===\n";
        std::cout << "Vertices: " << current_graph->getV() << "\n";
        std::cout << "Edges: " << current_graph->getE() << "\n";
        
        // Show first few vertices
        size_t show_count = std::min(current_graph->getV(), size_t(10));
        std::cout << "\nNeighbors (first " << show_count << " vertices):\n";
        for (size_t i = 0; i < show_count; ++i) {
            auto neighbors = current_graph->getNeighbors(i);
            std::cout << "  " << i << ": {";
            for (size_t j = 0; j < neighbors.size(); ++j) {
                std::cout << neighbors[j];
                if (j < neighbors.size() - 1) std::cout << ", ";
            }
            std::cout << "}\n";
        }
        
        if (current_graph->getV() > 10) {
            std::cout << "  ... and " << (current_graph->getV() - 10) << " more vertices\n";
        }
    }
    
    void generateGraph() {
        if (!selectBackend()) return;
        
        std::cout << "\n=== Graph Generators ===\n";
        std::cout << "1. Complete Graph (Kn)\n";
        std::cout << "2. Complete Bipartite (Knm)\n";
        std::cout << "3. Tree (Prufer)\n";
        std::cout << "4. Star (Sn)\n";
        std::cout << "5. Cycle (Cn)\n";
        std::cout << "6. Path (Pn)\n";
        std::cout << "7. Wheel (Wn)\n";
        std::cout << "8. Random Graph G(n,p)\n";
        std::cout << "9. Cubic Graph\n";
        std::cout << "10. Fixed Components\n";
        std::cout << "11. Forest\n";
        std::cout << "12. Fixed Bridges\n";
        std::cout << "13. Fixed Articulation Points\n";
        std::cout << "14. Fixed 2-Bridges\n";
        std::cout << "15. Halin Graph\n";
        std::cout << "Choice: ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        try {
            size_t n = getNumber("Enter n (number of vertices): ", 1);
            
            if (choice == "1") {
                GraphFactory::complete(*current_graph, n);
            } else if (choice == "2") {
                size_t m = getNumber("Enter m: ", 1);
                GraphFactory::completeBipartite(*current_graph, n, m);
            } else if (choice == "3") {
                GraphFactory::tree(*current_graph, n);
            } else if (choice == "4") {
                GraphFactory::star(*current_graph, n);
            } else if (choice == "5") {
                GraphFactory::cycle(*current_graph, n);
            } else if (choice == "6") {
                GraphFactory::path(*current_graph, n);
            } else if (choice == "7") {
                GraphFactory::wheel(*current_graph, n);
            } else if (choice == "8") {
                double p = getDouble("Enter p (probability 0-1): ", 0.0, 1.0);
                GraphFactory::random(*current_graph, n, p);
            } else if (choice == "9") {
                if (n % 2 != 0) {
                    std::cout << "Cubic graph requires even n!\n";
                    return;
                }
                GraphFactory::cubic(*current_graph, n);
            } else if (choice == "10") {
                size_t k = getNumber("Enter k (components): ", 1, n);
                GraphFactory::fixedComponents(*current_graph, n, k);
            } else if (choice == "11") {
                size_t k = getNumber("Enter k (trees in forest): ", 1, n);
                GraphFactory::forest(*current_graph, n, k);
            } else if (choice == "12") {
                size_t k = getNumber("Enter k (bridges): ", 0, n-1);
                GraphFactory::fixedBridges(*current_graph, n, k);
            } else if (choice == "13") {
                size_t k = getNumber("Enter k (articulation points): ", 0, n-2);
                GraphFactory::fixedArticulationPoints(*current_graph, n, k);
            } else if (choice == "14") {
                size_t k = getNumber("Enter k (2-bridges): ", 0);
                GraphFactory::fixedTwoBridges(*current_graph, n, k);
            } else if (choice == "15") {
                if (n < 4) {
                    std::cout << "Halin graph requires n >= 4!\n";
                    return;
                }
                GraphFactory::halin(*current_graph, n);
            } else {
                std::cout << "Invalid choice!\n";
                return;
            }
            
            std::cout << "Graph generated successfully!\n";
            showGraphInfo();
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
    
    void editGraph() {
        if (!current_graph) {
            std::cout << "No active graph! Create one first.\n";
            return;
        }
        
        while (true) {
            std::cout << "\n=== Edit Graph ===\n";
            std::cout << "1. Add Vertex\n";
            std::cout << "2. Add Edge\n";
            std::cout << "3. Remove Edge\n";
            std::cout << "4. Reallocate\n";
            std::cout << "5. Back to Main Menu\n";
            std::cout << "Choice: ";
            
            std::string choice;
            std::getline(std::cin, choice);
            
            if (choice == "5") break;
            
            try {
                if (choice == "1") {
                    current_graph->addVertices(1);
                    std::cout << "Vertex added. Total: " << current_graph->getV() << "\n";
                } else if (choice == "2") {
                    size_t u = getNumber("From vertex: ");
                    size_t v = getNumber("To vertex: ");
                    current_graph->addEdge(u, v);
                    std::cout << "Edge added (" << u << ", " << v << ")\n";
                } else if (choice == "3") {
                    size_t u = getNumber("From vertex: ");
                    size_t v = getNumber("To vertex: ");
                    size_t removed = current_graph->rmEdges(u, v);
                    std::cout << "Removed " << removed << " edge(s)\n";
                } else if (choice == "4") {
                    size_t n = getNumber("New size: ", 1);
                    current_graph->allocate(n);
                    std::cout << "Reallocated to " << n << " vertices\n";
                } else {
                    std::cout << "Invalid choice!\n";
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
    }
    
    void computeMetrics() {
        if (!current_graph) {
            std::cout << "No active graph!\n";
            return;
        }
        
        GraphMetrics metrics(*current_graph);
        
        std::cout << "\n=== Graph Metrics ===\n";
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Density: " << metrics.getDensity() << "\n";
        std::cout << "Diameter: " << metrics.getDiameter() << "\n";
        std::cout << "Transitivity: " << metrics.getTransitivity() << "\n";
        std::cout << "Components Count: " << metrics.getComponentsCount() << "\n";
        std::cout << "Articulation Points: " << metrics.getArticulationPointsCount() << "\n";
        std::cout << "Bridges (Tarjan): " << metrics.getBridgesCount() << "\n";
        std::cout << "Bridges (Randomized): " << metrics.getBridgesCountRandomized() << "\n";
        std::cout << "Is Bipartite: " << (metrics.isBipartite() ? "Yes" : "No") << "\n";
        std::cout << "Chromatic Number (upper bound): " << metrics.estimateChromaticNumber() << "\n";
    }
    
    void loadGraph() {
        if (!selectBackend()) return;
        
        std::cout << "\n=== Select Parser ===\n";
        std::cout << "1. Edge List\n";
        std::cout << "2. Adjacency Matrix\n";
        std::cout << "3. DIMACS\n";
        std::cout << "4. SNAP\n";
        std::cout << "Choice: ";
        
        std::string choice;
        std::getline(std::cin, choice);
        
        CurrentParser parser_type = CurrentParser::EDGE_LIST;
        if (choice == "2") parser_type = CurrentParser::ADJACENCY_MATRIX;
        else if (choice == "3") parser_type = CurrentParser::DIMACS;
        else if (choice == "4") parser_type = CurrentParser::SNAP;
        
        parser.reassignParser(parser_type);
        
        std::string filename = getUserInput("Enter filename: ");
        
        try {
            parser.parse(filename, *current_graph);
            std::cout << "Graph loaded successfully!\n";
            showGraphInfo();
        } catch (const std::exception& e) {
            std::cout << "Error loading graph: " << e.what() << "\n";
        }
    }
    
    void saveGraph() {
        if (!current_graph) {
            std::cout << "No active graph!\n";
            return;
        }
        
        std::cout << "\n=== Select Serialization Format ===\n";
        std::cout << "1. GraphViz (.dot)\n";
        std::cout << "2. Program4You (.edges)\n";
        std::cout << "Choice: ";
        
        std::string formatChoice;
        std::getline(std::cin, formatChoice);
        
        std::string filename = getUserInput("Enter output filename: ");
        
        try {
            if (formatChoice == "2") {
                // Program4You Serializer
                Program4YouSerializer serializer(*current_graph);
                serializer.saveToFile(filename);
                std::cout << "Saved to " << filename << " (Program4You format)\n";
            } else {
                // GraphViz Serializer
                std::cout << "\nGraphViz Options:\n";
                std::cout << "1. Simple graph\n";
                std::cout << "2. With custom title\n";
                std::cout << "Choice: ";
                
                std::string styleChoice;
                std::getline(std::cin, styleChoice);
                
                std::string title = "Graph";
                if (styleChoice == "2") {
                    title = getUserInput("Enter graph title: ");
                }
                
                GraphVizSerializer serializer(*current_graph);
                serializer.saveToFile(filename, title);
                std::cout << "Saved to " << filename << " (GraphViz format)\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error saving graph: " << e.what() << "\n";
        }
    }
    
    void showMainMenu() {
        std::cout << "\n╔══════════════════════════════════════╗\n";
        std::cout << "║       GraphoDro4 - Main Menu         ║\n";
        std::cout << "╠══════════════════════════════════════╣\n";
        std::cout << "║ 1. Create Empty Graph                ║\n";
        std::cout << "║ 2. Generate Graph                    ║\n";
        std::cout << "║ 3. Edit Graph                        ║\n";
        std::cout << "║ 4. Show Graph Info                   ║\n";
        std::cout << "║ 5. Compute Metrics                   ║\n";
        std::cout << "║ 6. Load Graph from File              ║\n";
        std::cout << "║ 7. Save Graph                        ║\n";
        std::cout << "║ 8. Exit                              ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << "Choice: ";
    }

public:
    void run() {
        std::cout << "╔═══════════════════════════════════════════╗\n";
        std::cout << "║     Welcome to GraphoDro4 v1.0            ║\n";
        std::cout << "║     Graph Analysis Library                ║\n";
        std::cout << "╚═══════════════════════════════════════════╝\n";
        
        while (true) {
            showMainMenu();
            
            std::string choice;
            std::getline(std::cin, choice);
            
            if (choice == "8") {
                std::cout << "Goodbye!\n";
                break;
            } else if (choice == "1") {
                if (selectBackend()) {
                    editGraph();
                }
            } else if (choice == "2") {
                generateGraph();
            } else if (choice == "3") {
                editGraph();
            } else if (choice == "4") {
                showGraphInfo();
            } else if (choice == "5") {
                computeMetrics();
            } else if (choice == "6") {
                loadGraph();
            } else if (choice == "7") {
                saveGraph();
            } else {
                std::cout << "Invalid choice! Try again.\n";
            }
        }
    }
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Включаем UTF-8 для вывода
    SetConsoleCP(CP_UTF8);       // Включаем UTF-8 для ввода
#endif
    try {
        ConsoleUI ui;
        ui.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}
