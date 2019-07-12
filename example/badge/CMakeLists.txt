cmake_minimum_required(VERSION 3.9)

add_executable(badge badge.c)
include_directories(badge "/usr/local/include")
include_directories(badge "../")
target_link_libraries(badge neoradio2)
