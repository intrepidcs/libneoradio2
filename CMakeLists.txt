cmake_minimum_required (VERSION 3.25)
 
project (libneoradio2_all LANGUAGES C CXX)

option(BUILD_PYTHON_BINDINGS "Enable Python bindings" OFF)

add_subdirectory(libneoradio2)

if (BUILD_PYTHON_BINDINGS) 
    add_subdirectory(python)
endif()