cmake_minimum_required(VERSION 3.9)

add_executable(basic basic.c)
include_directories(basic "/usr/local/include")
include_directories(basic "../../")
target_link_libraries(basic neoradio2)
