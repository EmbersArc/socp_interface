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
    src/expression.cpp
    src/constraint.cpp
    src/optimizationProblem.cpp
    src/secondOrderConeProgram.cpp
    src/ecosWrapper.cpp
)

# ECOS
# Make sure the repository is cloned with --recurse-submodules
execute_process(COMMAND make shared WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/solvers/ecos)
link_libraries(${CMAKE_CURRENT_LIST_DIR}/solvers/ecos/libecos.so)

add_library(socp_lib SHARED ${SOCP_SOURCES})
target_include_directories(socp_lib PUBLIC ${TARGET_INCLUDE})
target_include_directories(socp_lib SYSTEM PUBLIC ${TARGET_INCLUDE_SYS})

set(DEBUG_OPTIONS -Wall -Wextra -Wpedantic)
set(RELEASE_OPTIONS -O2)
target_compile_options(socp_lib PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(socp_lib PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")

target_link_libraries(socp_lib Eigen3::Eigen)

add_executable(socp_test src/tests/socp_test.cpp)
target_link_libraries(socp_test socp_lib)

add_executable(parameter_test src/tests/parameter_test.cpp)
target_link_libraries(parameter_test socp_lib)
