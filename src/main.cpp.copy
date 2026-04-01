#include <ncurses.h>

#include <string>
#include <vector>

/**
 * @brief Demonstration of a TUI menu using ncurses.
 * Logic: We highlight a "door" and trigger an action upon Enter.
 */
int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);  // Enable arrow keys

    int n = 5;  // Number of doors
    int highlight = 0;
    int choice = -1;

    while (true) {
        clear();
        mvprintw(
            0, 0,
            "Use arrows to select a door, Press ENTER to open, 'q' to quit:");

        for (int i = 0; i < n; ++i) {
            if (i == highlight)
                attron(A_REVERSE);  // Highlight the "active" button
            mvprintw(i + 2, 2, "door %d: L", i + 1);
            if (i == highlight) attroff(A_REVERSE);
        }

        if (choice != -1) {
            mvprintw(n + 4, 2, "Status: Opened door %d", choice + 1);
        }

        int c = getch();
        switch (c) {
            case KEY_UP:
                highlight = (highlight == 0) ? n - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == n - 1) ? 0 : highlight + 1;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            case 'q':
                endwin();
                return 0;
        }
    }

    endwin();
    return 0;
}
