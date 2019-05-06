cmake_minimum_required(VERSION 3.9)

project(neoradio2 VERSION 0.0.4 DESCRIPTION "C Library to interface to neoRAD-IO-2 Devices")

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include_directories(${PROJECT_SOURCE_DIR}/hidapi/hidapi)
add_library(neoradio2 SHARED ${PROJECT_SOURCE_DIR}/cmake_hack.cpp
    ${PROJECT_SOURCE_DIR}/fifo.c
    ${PROJECT_SOURCE_DIR}/device.cpp
    ${PROJECT_SOURCE_DIR}/hiddevice.cpp
    ${PROJECT_SOURCE_DIR}/libneoradio2.cpp
    ${PROJECT_SOURCE_DIR}/neoradio2device.cpp
    ${PROJECT_SOURCE_DIR}/hidapi/linux/hid.c)

set_target_properties(neoradio2 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER libneoradio2.h neoradio2common.h radio2_frames.h
    C_VISIBILITY hidden
    CXX_VISIBILITY hidden)

target_link_libraries(neoradio2 PRIVATE)
target_compile_options(neoradio2 PRIVATE)

configure_file(libneoradio2.pc.in libneoradio2.pc @ONLY)

install(TARGETS neoradio2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/libneoradio2.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
