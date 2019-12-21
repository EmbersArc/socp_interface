cmake_minimum_required(VERSION 3.5)
project(socp_interface)

set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

set(TARGET_INCLUDE
    include
)

set(TARGET_INCLUDE_SYS
    solvers/ecos/include
    solvers/ecos/external/SuiteSparse_config
)

set(SOCP_SOURCES
    src/parameter.cpp
    src/variable.cpp
    # src/expression.cpp
    # src/constraint.cpp
    # src/secondOrderConeProgram.cpp
    # src/ecosWrapper.cpp
)

# ECOS
# Make sure the repository is cloned with --recurse-submodules
execute_process(COMMAND make shared WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/solvers/ecos)
link_libraries(${CMAKE_CURRENT_LIST_DIR}/solvers/ecos/libecos.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wpedantic")

add_library(socp_lib SHARED ${SOCP_SOURCES})
target_include_directories(socp_lib PUBLIC ${TARGET_INCLUDE})
target_include_directories(socp_lib SYSTEM PUBLIC ${TARGET_INCLUDE_SYS})

if(${EIGEN3_FOUND})
    message("Eigen found.")
    add_definitions(-DEIGEN_AVAILABLE)
endif()

target_link_libraries(socp_lib Eigen3::Eigen)

add_executable(example src/example.cpp)
target_link_libraries(example socp_lib)
