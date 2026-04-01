cmake_minimum_required(VERSION 3.15)
project(GraphoDro4 VERSION 1.0)

# --- астройки компиляции ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Поиск исходных файлов для библиотеки ---
# Собираем всё из src, кроме main.cpp
file(GLOB_RECURSE LIB_SOURCES
    "src/algorithms/*.cpp"
    "src/core/*.cpp"
    "src/generators/*.cpp"
    "src/parsers/*.cpp"
    "src/serializer/*.cpp"
)
# На всякий случай явно исключаем main из списка объектов библиотеки
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

file(GLOB_RECURSE HEADERS "include/*.hpp")

# --- 1. Создание статической библиотеки ---
add_library(graph_lib STATIC ${LIB_SOURCES} ${HEADERS})

# add_library(graphlib STATIC ${LIB_SOURCES} ${LIB_HEADERS})
# target_include_directories(graphlib PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
#     ${CMAKE_CURRENT_SOURCE_DIR}/external
# )

# PUBLIC гарантирует, что любой, кто линкуется с graph_lib,
# автоматически добавит папку include в пути поиска (решает проблему fatal error)
target_include_directories(graph_lib PUBLIC include)

# --- 2. Создание основного исполняемого файла ---
add_executable(graphodro4 src/main.cpp)
target_link_libraries(graphodro4 PRIVATE graph_lib)

# Настройка вывода бинарника в bin/
set_target_properties(graphodro4 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- 3. Флаги компилятора ---
if(MSVC)
    target_compile_options(graph_lib PRIVATE /W4)
else()
    target_compile_options(graph_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- 4. Подключение тестов ---
enable_testing()
add_subdirectory(tests)

message(STATUS "Project ${PROJECT_NAME} configured. Library includes: algorithms, core, generators, parsers, serializers.")
