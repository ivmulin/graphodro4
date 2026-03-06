cmake_minimum_required(VERSION 3.15)
project(GraphoDro4 VERSION 1.0)

# ProjectSettings: Установка стандарта C++17 и отключение расширений
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# BuildTypeManagement: Установка Release как типа сборки по умолчанию
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "No build type selected, defaulting to Release")
endif()

# SourceDiscovery: Автоматический поиск всех исходников и заголовков
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

# OutputDirectoryConfiguration: Исполняемый файл будет создан в корне проекта
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# ExecutableTargetCreation: Определение финального бинарника
add_executable(graphodro4 ${SOURCES} ${HEADERS})

# IncludePathConfiguration: Добавление папки include в пути поиска
target_include_directories(graphodro4 PRIVATE include)

# CompilerWarningConfiguration: Настройка строгого контроля предупреждений
if(MSVC)
    target_compile_options(graphodro4 PRIVATE /W4)
else()
    target_compile_options(graphodro4 PRIVATE -Wall -Wextra -Wpedantic)
endif()
