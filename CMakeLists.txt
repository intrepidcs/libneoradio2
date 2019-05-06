cmake_minimum_required(VERSION 3.9)


add_definitions(-DLIBNEORADIO2_EXPORTS)

file(READ "version.txt" ver)

string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ "${ver}")
set(ver_major ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ "${ver}")
set(ver_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ "${ver}")
set(ver_patch ${CMAKE_MATCH_1})

message("VERSION DETECTED: ${ver_major}.${ver_minor}.${ver_patch}")

project(neoradio2 
    VERSION "${ver_major}.${ver_minor}.${ver_patch}" 
    DESCRIPTION "C Library to interface to neoRAD-IO-2 Devices")

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include_directories(${PROJECT_SOURCE_DIR}/hidapi/hidapi)
add_library(neoradio2 SHARED
    ${PROJECT_SOURCE_DIR}/fifo.c
    ${PROJECT_SOURCE_DIR}/device.cpp
    ${PROJECT_SOURCE_DIR}/hiddevice.cpp
    ${PROJECT_SOURCE_DIR}/libneoradio2.cpp
    ${PROJECT_SOURCE_DIR}/neoradio2device.cpp)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_sources(neoradio2 PRIVATE ${PROJECT_SOURCE_DIR}/hidapi/linux/hid.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(neoradio2 PRIVATE ${PROJECT_SOURCE_DIR}/hidapi/windows/hid.c)
endif()

set_target_properties(neoradio2 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    C_VISIBILITY hidden
    CXX_VISIBILITY hidden)

set_property(TARGET neoradio2 PROPERTY PUBLIC_HEADER libneoradio2.h libneoradio2common.h radio2_frames.h)

target_link_libraries(neoradio2 PRIVATE udev)

configure_file(libneoradio2.pc.in libneoradio2.pc @ONLY)

install(TARGETS neoradio2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/libneoradio2.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
