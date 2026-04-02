#include <ncurses.h>

#include <graphodro4/core/adjacency_list.hpp>
#include <graphodro4/core/adjacency_matrix.hpp>
#include <graphodro4/core/graphs.hpp>
#include <graphodro4/generators/generator.hpp>
#include <graphodro4/parsers/parsers.hpp>
#include <memory>
#include <string>
#include <vector>

#include "graphodro4/serializer/graphviz_serializer.hpp"

// Указатель на текущий активный граф
std::unique_ptr<IGraph> current_graph = nullptr;

enum HIGHLIGHT { NEW_GRAPH, GENERATE_GRAPH, EDIT, LOAD, DUMP };

enum EDITOR { ADD_VERTEX, RM_VERTEX, ADD_EDGE, RM_EDGE, REALLOCATE, EXIT };

/**
 * Вспомогательная функция для вывода сообщений в центре экрана.
 * Закрывается по нажатию любой клавиши.
 */
void show_message(const std::string& msg) {
    int h, w;
    getmaxyx(stdscr, h, w);
    int msg_w = msg.length() + 4;
    WINDOW* win = newwin(3, msg_w, h / 2 - 1, (w - msg_w) / 2);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "%s", msg.c_str());
    wrefresh(win);

    wgetch(win);
    delwin(win);
    touchwin(stdscr);
    refresh();
}

/**
 * Чтение строки от пользователя.
 */
std::string get_input_string(WINDOW* win, int y, int x,
                             const std::string& prompt) {
    echo();
    curs_set(1);
    char input[256];
    mvwprintw(win, y, x, "%s", prompt.c_str());
    wgetstr(win, input);
    noecho();
    curs_set(0);
    return std::string(input);
}

/**
 * Отрисовка текущего состояния графа в правом окне.
 */
void draw_graph_view(WINDOW* right_win) {
    werase(right_win);
    box(right_win, 0, 0);
    mvwprintw(right_win, 0, 2, " Graph View ");

    if (current_graph) {
        mvwprintw(right_win, 2, 2, "Vertices: %zu", current_graph->getV());
        mvwprintw(right_win, 3, 2, "Edges:    %zu", current_graph->getE());

        // Визуализация соседей для первых нескольких вершин
        size_t min_to_show = std::min(current_graph->getV(), (size_t)15);
        for (size_t i = 0; i < min_to_show; ++i) {
            auto neighbors = current_graph->getNeighbors(i);
            mvwprintw(right_win, 5 + i, 2, "%zu: ", i);
            for (auto n : neighbors) wprintw(right_win, "%zu ", n);
        }
        if (current_graph->getV() != (size_t)15) {
            size_t diff =
                std::max(current_graph->getV(), (size_t)15) - min_to_show;
            mvwprintw(right_win, 21, 2, "%zu more . . . ", diff);
        }
    } else {
        mvwprintw(right_win, 2, 2, "No active graph.");
    }
    wrefresh(right_win);
}

/**
 * Меню выбора типа графа (Backend).
 * Закрывается по Enter (выбор) или Esc (отмена).
 */
bool select_graph_backend() {
    int h, w;
    getmaxyx(stdscr, h, w);
    WINDOW* sub = newwin(8, 40, h / 2 - 4, w / 2 - 20);
    keypad(sub, TRUE);
    box(sub, 0, 0);

    std::vector<std::string> options = {"Adjacency List", "Adjacency Matrix"};
    int choice = 0;
    bool selected = false;

    while (true) {
        mvwprintw(sub, 1, 2, "Select Storage Type (ESC to cancel):");
        for (int i = 0; i < (int)options.size(); ++i) {
            if (i == choice) wattron(sub, A_REVERSE);
            mvwprintw(sub, 3 + i, 5, options[i].c_str());
            wattroff(sub, A_REVERSE);
        }
        wrefresh(sub);

        int ch = wgetch(sub);
        if (ch == 27) break;  // ESC
        if (ch == KEY_UP)
            choice = (choice > 0) ? choice - 1 : options.size() - 1;
        if (ch == KEY_DOWN)
            choice = (choice < (int)options.size() - 1) ? choice + 1 : 0;
        if (ch == 10) {  // Enter
            if (choice == 0)
                current_graph = std::make_unique<AdjacencyList>();
            else
                current_graph = std::make_unique<AdjacencyMatrix>();
            selected = true;
            break;
        }
    }
    delwin(sub);
    touchwin(stdscr);
    refresh();
    return selected;
}

/**
 * Режим редактирования графа.
 */
void draw_editor(WINDOW* left_win, WINDOW* right_win) {
    const std::vector<std::string> edit_menu = {
        "Add Vertex", "Add Edge", "Remove Edge", "Reallocate", "Back to Menu"};
    int highlight = 0;

    while (true) {
        draw_graph_view(right_win);
        werase(left_win);
        box(left_win, 0, 0);
        mvwprintw(left_win, 0, 2, " Editor Mode ");

        for (int i = 0; i < (int)edit_menu.size(); ++i) {
            if (i == highlight) wattron(left_win, A_REVERSE);
            mvwprintw(left_win, 2 + i, 4, edit_menu[i].c_str());
            wattroff(left_win, A_REVERSE);
        }
        wrefresh(left_win);

        int c = wgetch(left_win);
        if (c == 27) return;

        switch (c) {
            case KEY_UP:
                highlight =
                    (highlight > 0) ? highlight - 1 : edit_menu.size() - 1;
                break;
            case KEY_DOWN:
                highlight =
                    (highlight < (int)edit_menu.size() - 1) ? highlight + 1 : 0;
                break;
            case 10:                   // Enter
                if (highlight == 0) {  // Add Vertex
                    current_graph->addVertices(1);
                } else if (highlight == 1) {  // Add Edge (теперь индекс 1)
                    try {
                        size_t u = std::stoul(
                            get_input_string(left_win, 10, 2, "From: "));
                        size_t v = std::stoul(
                            get_input_string(left_win, 11, 2, "To: "));
                        current_graph->addEdge(u, v);
                    } catch (...) {
                        show_message("Error adding edge!");
                    }
                } else if (highlight == 2) {  // Remove Edge (теперь индекс 2)
                    try {
                        size_t u = std::stoul(
                            get_input_string(left_win, 10, 2, "From: "));
                        size_t v = std::stoul(
                            get_input_string(left_win, 11, 2, "To: "));
                        current_graph->rmEdges(u, v);
                    } catch (...) {
                        show_message("Error removing edge!");
                    }
                } else if (highlight == 3) {  // Reallocate (теперь индекс 3)
                    try {
                        std::string n_str = get_input_string(
                            left_win, 10, 2, "New capacity (n): ");
                        size_t n = std::stoul(n_str);
                        current_graph->allocate(n);
                        show_message("Reallocated to " + std::to_string(n) +
                                     " vertices.");
                    } catch (...) {
                        show_message("Invalid number!");
                    }
                } else if (highlight == 4)
                    return;  // Back to Menu
                break;
        }
    }
}

void draw_generate_menu(WINDOW* left_win, WINDOW* right_win) {
    if (!select_graph_backend()) return;

    int h, w;
    getmaxyx(stdscr, h, w);
    // Увеличиваем высоту окна для размещения всех опций
    WINDOW* gen_win = newwin(20, 50, h / 2 - 10, w / 2 - 25);
    keypad(gen_win, TRUE);
    box(gen_win, 0, 0);

    // Полный список согласно generator.hpp
    std::vector<std::string> gen_types = {"Complete Graph",
                                          "Complete Bipartite",
                                          "Tree (Prufer)",
                                          "Star",
                                          "Cycle",
                                          "Path",
                                          "Wheel",
                                          "Random (Erdos-Renyi)",
                                          "Cubic",
                                          "Fixed Components",
                                          "Forest",
                                          "Fixed Bridges",
                                          "Fixed Articulation Points",
                                          "Fixed 2-Bridges",
                                          "Halin Graph"};
    int highlight = 0;

    while (true) {
        werase(gen_win);
        box(gen_win, 0, 0);
        mvwprintw(gen_win, 1, 2, "Select Generator (ESC to cancel):");

        for (int i = 0; i < (int)gen_types.size(); ++i) {
            if (i == highlight) wattron(gen_win, A_REVERSE);
            mvwprintw(gen_win, 3 + i, 4, gen_types[i].c_str());
            wattroff(gen_win, A_REVERSE);
        }
        wrefresh(gen_win);

        int ch = wgetch(gen_win);
        if (ch == 27) break;
        if (ch == KEY_UP)
            highlight = (highlight > 0) ? highlight - 1 : gen_types.size() - 1;
        if (ch == KEY_DOWN)
            highlight =
                (highlight < (int)gen_types.size() - 1) ? highlight + 1 : 0;

        if (ch == 10) {
            try {
                size_t n = std::stoul(get_input_string(gen_win, 18, 2, "n: "));

                // Обработка каждого типа генератора
                if (highlight == 0)
                    GraphFactory::complete(*current_graph, n);
                else if (highlight == 1) {
                    size_t m =
                        std::stoul(get_input_string(gen_win, 18, 15, "m: "));
                    GraphFactory::completeBipartite(*current_graph, n, m);
                } else if (highlight == 2)
                    GraphFactory::tree(*current_graph, n);
                else if (highlight == 3)
                    GraphFactory::star(*current_graph, n);
                else if (highlight == 4)
                    GraphFactory::cycle(*current_graph, n);
                else if (highlight == 5)
                    GraphFactory::path(*current_graph, n);
                else if (highlight == 6)
                    GraphFactory::wheel(*current_graph, n);
                else if (highlight == 7) {
                    double p =
                        std::stod(get_input_string(gen_win, 18, 15, "p: "));
                    GraphFactory::random(*current_graph, n, p);
                } else if (highlight == 8)
                    GraphFactory::cubic(*current_graph, n);
                else if (highlight == 9) {
                    size_t k =
                        std::stoul(get_input_string(gen_win, 18, 15, "k: "));
                    GraphFactory::fixedComponents(*current_graph, n, k);
                } else if (highlight == 10) {
                    size_t k =
                        std::stoul(get_input_string(gen_win, 18, 15, "k: "));
                    GraphFactory::forest(*current_graph, n, k);
                } else if (highlight == 11) {
                    size_t k =
                        std::stoul(get_input_string(gen_win, 18, 15, "k: "));
                    GraphFactory::fixedBridges(*current_graph, n, k);
                } else if (highlight == 12) {
                    size_t k =
                        std::stoul(get_input_string(gen_win, 18, 15, "k: "));
                    GraphFactory::fixedArticulationPoints(*current_graph, n, k);
                } else if (highlight == 13) {
                    size_t k =
                        std::stoul(get_input_string(gen_win, 18, 15, "k: "));
                    GraphFactory::fixedTwoBridges(*current_graph, n, k);
                } else if (highlight == 14)
                    GraphFactory::halin(*current_graph, n);

                show_message("Graph generated!");
                delwin(gen_win);
                draw_editor(left_win, right_win);
                return;
            } catch (const std::exception& e) {
                show_message(e.what());
            }
        }
    }
    delwin(gen_win);
    touchwin(stdscr);
    refresh();
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    ParserUtility parser(CurrentParser::EDGE_LIST);

    int h, w;
    getmaxyx(stdscr, h, w);

    WINDOW* left_win = nullptr;
    WINDOW* right_win = nullptr;

    // 1. Проверка на критически малый размер
    if (h < 10 || w < 30) {
        endwin();
        printf("Oops! It's too narrow here. Please resize your terminal.\n");
        return 1;  // Завершаем работу, так как интерфейс не влезет
    }

    // 2. Логика разделения
    if (w >= 80) {
        // Горизонтальное разделение (Side-by-side)
        int left_w = (w * 2) / 5;
        left_win = newwin(h, left_w, 0, 0);
        right_win = newwin(h, w - left_w, 0, left_w);
    } else {
        // Вертикальное разделение (Stacked)
        // Оставляем под меню фиксированную высоту или пропорцию
        int menu_h = 20;
        left_win = newwin(menu_h, w, 0, 0);
        right_win = newwin(h - menu_h, w, menu_h, 0);
    }

    // 3. Безопасная настройка
    if (left_win) keypad(left_win, TRUE);
    if (right_win) keypad(right_win, TRUE);

    std::vector<std::string> main_menu = {"Create empty graph",
                                          "Generate graph", "Edit graph",
                                          "Load from file", "Dump to GraphViz"};
    int highlight = 0;

    while (true) {
        draw_graph_view(right_win);
        werase(left_win);
        box(left_win, 0, 0);
        mvwprintw(left_win, 0, 2, " Main Menu ");

        for (int i = 0; i < (int)main_menu.size(); ++i) {
            if (i == highlight) wattron(left_win, A_REVERSE);
            mvwprintw(left_win, 2 + i, 4, main_menu[i].c_str());
            wattroff(left_win, A_REVERSE);
        }
        wrefresh(left_win);

        int c = wgetch(left_win);
        switch (c) {
            case KEY_UP:
                highlight =
                    (highlight > 0) ? highlight - 1 : main_menu.size() - 1;
                break;
            case KEY_DOWN:
                highlight =
                    (highlight < (int)main_menu.size() - 1) ? highlight + 1 : 0;
                break;
            case 'q':
                endwin();
                return 0;
            case 10:
                if (highlight == HIGHLIGHT::NEW_GRAPH) {
                    if (select_graph_backend())
                        draw_editor(left_win, right_win);
                } else if (highlight == HIGHLIGHT::GENERATE_GRAPH) {
                    draw_generate_menu(left_win, right_win);  // Новая логика
                } else if (highlight == HIGHLIGHT::LOAD) {
                    if (select_graph_backend()) {
                        std::string path =
                            get_input_string(left_win, 12, 2, "Path: ");
                        try {
                            parser.parse(path, *current_graph);
                            show_message("Loaded!");
                            draw_editor(left_win, right_win);
                        } catch (...) {
                            show_message("Failed!");
                        }
                    }
                } else if (highlight == HIGHLIGHT::EDIT) {
                    if (current_graph) {
                        draw_editor(left_win, right_win);
                    } else {
                        show_message("No graph loaded :(");
                    }
                } else if (highlight == HIGHLIGHT::DUMP) {
                    if (current_graph) {
                        std::string path =
                            get_input_string(left_win, 12, 2, "Save as: ");
                        try {
                            GraphVizSerializer serializer(*current_graph);
                            serializer.saveToFile(path);
                            show_message("Saved to " + path);
                        } catch (...) {
                            show_message("Save failed!");
                        }
                    } else {
                        show_message("Nothing to save :P");
                    }
                }
                break;
        }
    }
    endwin();
    return 0;
}
