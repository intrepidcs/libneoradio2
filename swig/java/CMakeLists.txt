cmake_minimum_required(VERSION 3.9)

find_package(Java REQUIRED)
#find_package(JNI REQUIRED)
include(UseJava)
#include_directories(${JNI_INCLUDE_DIRS})
include_directories(${_JAVA_HOME}/include
    ${_JAVA_HOME}/include/linux
)

file(READ "../../version.txt" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ "${ver}")
set(ver_major ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ "${ver}")
set(ver_minor ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ "${ver}")
set(ver_patch ${CMAKE_MATCH_1})
message("VERSION DETECTED: ${ver_major}.${ver_minor}.${ver_patch}")

project(neoradio2 
    VERSION "${ver_major}.${ver_minor}.${ver_patch}" 
    DESCRIPTION "Java Library to interface to neoRAD-IO-2 Devices")

add_library(neoradio2 SHARED
    java/swig_wrap.cxx
)

set_target_properties(neoradio2 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    C_VISIBILITY hidden
    CXX_VISIBILITY hidden)
