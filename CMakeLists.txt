cmake_minimum_required(VERSION 3.15)
project(GraphoDro4 VERSION 1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Настройки компиляции ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Установка типа сборки по умолчанию (Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "No build type selected, defaulting to Release")
endif()

# --- Поиск исходных файлов ---
# Рекурсивно собираем все .cpp и .hpp для участия в сборке и отображения в IDE
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

# --- Создание исполняемого файла ---
add_executable(graphodro4 ${SOURCES} ${HEADERS})

# --- Настройка путей инклудов ---
# Мы указываем корень папки include.
# Благодаря структуре include/graphodro4/... обращения в коде будут вида:
# #include <graphodro4/core/AdjacencyMatrix.hpp>
target_include_directories(graphodro4 PRIVATE include)

# --- Настройка вывода ---
# Размещаем готовый бинарник в папку bin внутри директории сборки
set_target_properties(graphodro4 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- Флаги компилятора ---
if(MSVC)
    target_compile_options(graphodro4 PRIVATE /W4)
else()
    target_compile_options(graphodro4 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Информационное сообщение при генерации
message(STATUS "Project ${PROJECT_NAME} configured. Output will be in: ${CMAKE_BINARY_DIR}/bin")
