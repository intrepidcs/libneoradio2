find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

option(BUILD_JAVA, "Build Java SWIG module" ON)
if (BUILD_JAVA)
	add_subdirectory(java)
endif()

option(BUILD_CSHARP, "Build CSHARP SWIG module" ON)
if (BUILD_CSHARP)
	add_subdirectory(java)
endif()

