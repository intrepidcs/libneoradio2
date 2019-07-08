cmake_minimum_required(VERSION 3.10)
project(neoRAD_IO2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/cpplib/main.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
add_subdirectory(neoRAD-IO2-C-Library)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC} ${HIDAPI_DIR})
target_link_libraries(${PROJECT_NAME} neoRADIO2 ${CMAKE_JS_LIB})
