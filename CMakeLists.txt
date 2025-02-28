cmake_minimum_required (VERSION 3.25)
 
project (libneoradio2_all LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_PYTHON_BINDINGS "Enable Python bindings" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if (APPLE)
  target_link_libraries(neoradio2 "-framework CoreFoundation")
  target_link_libraries(neoradio2 "-framework IOKit")
  target_link_libraries(neoradio2 hidapi)  # Ensure HIDAPI is linked
  set_target_properties(neoradio2 PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")
endif()

add_subdirectory(libneoradio2)

if (BUILD_PYTHON_BINDINGS) 
    add_subdirectory(python)
endif()