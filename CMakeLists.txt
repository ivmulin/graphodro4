cmake_minimum_required(VERSION 3.15)
project(GraphoDro4 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "No build type selected, defaulting to Release")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(LIB_SOURCES
    src/adjacency_list.cpp
    src/parsers.cpp
    src/graphviz_serializer.cpp
)

set(LIB_HEADERS
    include/igraph.hpp
    include/adjacency_list.hpp
    include/parsers.hpp
    include/graphviz_serializer.hpp
    include/_debugging.hpp
)

add_library(graphlib STATIC ${LIB_SOURCES} ${LIB_HEADERS})
target_include_directories(graphlib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)

add_executable(graphodro4 src/graphodro4.cpp)
target_link_libraries(graphodro4 PRIVATE graphlib)

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

if(TEST_SOURCES)
    add_executable(test_serializer 
        ${TEST_SOURCES}
        # graphodro4.cpp НЕ включать! Только библиотека
    )
    target_link_libraries(test_serializer PRIVATE graphlib)
    
    enable_testing()
    add_test(NAME GraphVizSerializerTests COMMAND test_serializer)
    
    add_custom_target(run_tests
        COMMAND test_serializer
        DEPENDS test_serializer
        COMMENT "Running all tests..."
    )
    
    message(STATUS "Tests found: ${TEST_SOURCES}")
else()
    message(STATUS "No tests found in 'tests/' directory")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/samples")
    add_custom_command(TARGET graphodro4 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/samples
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples
        COMMENT "Copying samples/ for graphodro4"
    )
    if(TEST_SOURCES)
        add_custom_command(TARGET test_serializer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/samples
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/samples
            COMMENT "Copying samples/ for tests"
        )
    endif()
endif()

message(STATUS "Build configuration complete!")