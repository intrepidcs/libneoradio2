cmake_minimum_required(VERSION 3.15...3.29)
project(neoradio2 LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${PROJECT_NAME} src/main.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

# Propagate the package version (set by scikit-build-core from pyproject) so
# neoradio2.__version__ reports the real version instead of "dev".
if(DEFINED SKBUILD_PROJECT_VERSION)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION_INFO=${SKBUILD_PROJECT_VERSION})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE libneoradio2)

install(TARGETS ${PROJECT_NAME} DESTINATION .)