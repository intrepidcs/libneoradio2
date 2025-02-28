cmake_minimum_required (VERSION 3.25)
 
project (libneoradio2_all LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_sources(neoradio2 PRIVATE ${PROJECT_SOURCE_DIR}/hidapi/linux/hid.c)
    target_link_libraries(neoradio2 PRIVATE udev hidapi)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_sources(neoradio2 PRIVATE ${PROJECT_SOURCE_DIR}/hidapi/windows/hid.c)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")  # MacOS
    target_sources(neoradio2 PRIVATE ${PROJECT_SOURCE_DIR}/hidapi/mac/hid.c)
    target_link_libraries(neoradio2 PRIVATE "-framework CoreFoundation" "-framework IOKit" hidapi)
endif()

option(BUILD_PYTHON_BINDINGS "Enable Python bindings" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_subdirectory(libneoradio2)

if (BUILD_PYTHON_BINDINGS) 
    add_subdirectory(python)
endif()