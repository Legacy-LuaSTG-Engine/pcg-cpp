cmake_minimum_required(VERSION 3.15)
project(pcg_cpp)

add_library(pcg_cpp INTERFACE)
target_include_directories(pcg_cpp INTERFACE include)

add_executable(pcg_demo)
target_sources(pcg_demo PRIVATE sample/pcg-demo.cpp)
target_link_libraries(pcg_demo PRIVATE pcg_cpp)

# Tests

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()
if (BUILD_TESTING)
    add_subdirectory(test-high)
endif ()
