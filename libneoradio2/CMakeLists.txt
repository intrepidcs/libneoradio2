cmake_minimum_required(VERSION 3.25)
project(libneoradio2 VERSION 1.4.1 LANGUAGES C CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This static archive (and the hidapi subdirectory below) is linked into the
# pybind11 shared module, so all objects must be position-independent or the
# Linux/macOS wheel link fails with "recompile with -fPIC".
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

# Only define the dllexport macro when actually building a shared library, and
# keep it private to this target instead of leaking into the hidapi objects.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LIBNEORADIO2_EXPORTS)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hidapi)
# Use the namespaced target: hidapi only defines a plain `hidapi` alias on
# Windows/macOS, so linking `hidapi` resolves to a bogus `-lhidapi` on Linux.
# `hidapi::hidapi` is defined on every platform.
target_link_libraries(${PROJECT_NAME} hidapi::hidapi)

# Install rules for using the C library from an install tree (e.g. the
# `make install` documented in the README). Skipped inside the Python wheel
# build (scikit-build-core sets SKBUILD) so the static lib/headers aren't
# bundled into the wheel.
if(NOT SKBUILD)
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libneoradio2
        FILES_MATCHING PATTERN "*.h")
endif()
