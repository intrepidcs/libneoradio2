cmake_minimum_required(VERSION 3.25)
project(libneoradio2 VERSION 1.3.1 LANGUAGES C CXX)

add_definitions(-DLIBNEORADIO2_EXPORTS)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(IncludeDirectories
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/hidapi/hidapi
    ${CMAKE_CURRENT_SOURCE_DIR}/neoRAD-IO2-FrameDescription)

set(SourceFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hiddevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libneoradio2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/neoradio2device.cpp)

add_library(${PROJECT_NAME} ${SourceFiles})
target_include_directories(${PROJECT_NAME} PUBLIC ${IncludeDirectories})


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hidapi)
target_link_libraries(${PROJECT_NAME} hidapi)
