include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.13.0
)
FetchContent_MakeAvailable(Catch2)

# Файлы тестов
add_executable(unit_tests
    test_main.cpp       # Точка входа для Catch2
    test_core.cpp       # Тесты функционала графов
    test_metrics.cpp    # Тесты для GraphMetrics
    test_generators.cpp # Тесты для проверки инвариантов генераторов

)

# Линкуемся к библиотеке и Catch2
target_link_libraries(unit_tests PRIVATE
    graph_lib
    Catch2::Catch2WithMain
)

# Чтобы CTest видел тесты Catch2
include(Catch)
catch_discover_tests(unit_tests)
