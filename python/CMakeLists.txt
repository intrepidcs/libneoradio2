cmake_minimum_required(VERSION 3.15...3.29)
project(neoradio2 LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE libneoradio2)

install(TARGETS ${PROJECT_NAME} DESTINATION .)