cmake_minimum_required (VERSION 3.1)

project (bx)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions (GRAPHICS_VK_BACKEND)

# TODO: Use Platform.hpp for the future
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_compile_definitions (PLATFORM_PC)
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_compile_definitions (PLATFORM_LINUX)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_definitions (ENGINE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
	add_compile_definitions (DEBUG_BUILD)
endif ()
# ---------------------------------------

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (BX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory (extern)
add_subdirectory (scripts)
add_subdirectory (src)

add_subdirectory (demo)