cmake_minimum_required(VERSION 3.15)
project(GraphoDro4 VERSION 1.0)

# --- Глобальные настройки ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# КРИТИЧНО: Решает проблему DT_TEXTREL в PIE на Arch Linux
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Поиск ncurses (Кроссплатформенно) ---
if(WIN32)
    # Поиск PDCurses для Windows
    find_path(CURSES_INCLUDE_DIR NAMES curses.h)
    find_library(CURSES_LIBRARY NAMES pdcurses)
    if(NOT CURSES_LIBRARY)
        message(WARNING "PDCurses not found. UI might not compile on Windows.")
    endif()
else()
    # Стандартный поиск ncurses для Linux/macOS
    find_package(Curses REQUIRED)
endif()

# --- Исходники библиотеки ---
file(GLOB_RECURSE LIB_SOURCES 
    "src/algorithms/*.cpp" 
    "src/core/*.cpp" o
    "src/generators/*.cpp" 
    "src/parsers/*.cpp" 
    "src/serializer/*.cpp"
)
# Исключаем main, чтобы он не дублировался при линковке тестов
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/main.cpp")

# --- Создание библиотеки ---
add_library(graph_lib STATIC ${LIB_SOURCES})
target_include_directories(graph_lib PUBLIC include)

# --- Основное приложение ---
add_executable(graphodro4 src/main.cpp)

# Линковка: библиотека графов + ncurses
target_link_libraries(graphodro4 PRIVATE 
    graph_lib 
    ${CURSES_LIBRARIES}
)

# Если ncurses требует специфических путей к include
if(CURSES_INCLUDE_DIR)
    target_include_directories(graphodro4 PRIVATE ${CURSES_INCLUDE_DIR})
endif()

# --- Подключение тестов ---
enable_testing()
add_subdirectory(tests)
