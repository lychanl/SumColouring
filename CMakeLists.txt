cmake_minimum_required(VERSION 2.6)

project(SumColouring)

set(CMAKE_CXX_STANDARD 11)
set(PROJECT_SOURCE_DIR src)
set(EXECUTABLE_OUTPUT_PATH out)

add_executable(colour 
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/Solver.cpp
    ${PROJECT_SOURCE_DIR}/LinearProgrammingSolver.cpp
    ${PROJECT_SOURCE_DIR}/GISSolver.cpp)

target_include_directories(colour PRIVATE eigen/Eigen)
