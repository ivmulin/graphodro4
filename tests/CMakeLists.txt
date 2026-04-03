include(FetchContent)

# Скачиваем Catch2 v3
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.13.0 
)
FetchContent_MakeAvailable(Catch2)

# Список файлов тестов
add_executable(unit_tests
    test_core.cpp
    test_metrics.cpp
    test_generators.cpp
    test_serializer.cpp
)

# Линковка тестов
# Нам НЕ НУЖЕН ncurses для тестов ядра, если только ты не тестируешь UI
target_link_libraries(unit_tests PRIVATE
    graph_lib
    Catch2::Catch2WithMain
)



# Интеграция с CTest (позволяет запускать 'make test' или 'ctest')
include(Catch)
catch_discover_tests(unit_tests)
