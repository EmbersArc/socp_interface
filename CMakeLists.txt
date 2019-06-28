cmake_minimum_required(VERSION 3.5)
project(scop_interface)

set(CMAKE_CXX_STANDARD 17)

# ECOS
execute_process(COMMAND make shared WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/solvers/ecos)
link_libraries(${CMAKE_CURRENT_LIST_DIR}/solvers/ecos/libecos.so)

if(DEFINED ENV{MOSEK_ROOT})
    set(MOSEK_AVAILABLE true)
endif()

# Use MOSEK if available
# You need to set the environment variable: e.g. 'export MOSEK_ROOT=.../mosek/9.0/tools/platform/linux64x86'
if(MOSEK_AVAILABLE)
    message("MOSEK_ROOT: " $ENV{MOSEK_ROOT})
    execute_process(COMMAND make install WORKING_DIRECTORY $ENV{MOSEK_ROOT}/src/fusion_cxx)
    link_libraries($ENV{MOSEK_ROOT}/bin/libmosek64.so)
    link_libraries($ENV{MOSEK_ROOT}/bin/libfusion64.so)
else()
    message("MOSEK_ROOT is not defined. Mosek will not be available.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(TARGET_INCLUDE
    include
)
set(TARGET_INCLUDE_SYS
    solvers/ecos/include
    solvers/ecos/external/SuiteSparse_config
)
set(SCOP_SOURCES
    src/ecosWrapper.cpp
    src/optimizationProblem.cpp
)

if(MOSEK_AVAILABLE)
    set(TARGET_INCLUDE_SYS
        ${TARGET_INCLUDE_SYS}
        $ENV{MOSEK_ROOT}/h
        )
    set(SCOP_SOURCES
        ${SCOP_SOURCES}
        src/mosekWrapper.cpp
        )
endif()

add_library(scop_lib SHARED ${SCOP_SOURCES})
target_include_directories(scop_lib PUBLIC ${TARGET_INCLUDE})
target_include_directories(scop_lib SYSTEM PUBLIC ${TARGET_INCLUDE_SYS})
