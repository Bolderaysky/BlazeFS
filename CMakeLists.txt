cmake_minimum_required(VERSION 3.15)

project(BlazeFS VERSION 1.0
        DESCRIPTION "Library that allows you to use virtual filesystem that can be loaded from asar on drive or directly from memory"
        LANGUAGES CXX)

include(CheckIPOSupported)
include(cmake/AddGitSubmodule.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_PATH "src" CACHE STRING "Source")
set(INCLUDE_PATH "include" CACHE STRING "Headers")
set(EXAMPLES_PATH "examples" CACHE STRING "Examples for the filesystem")
set(DEFAULT_BUILD_TYPE "Release" CACHE STRING "Define default build type")
option(BUILD_EXAMPLES "Boolean that specifies if it's needed to build examples or not" ON)
option(BUILD_TESTS "Boolean that specifies if it's needed to build tests or not" ON)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

add_git_submodule("${PROJECT_SOURCE_DIR}/dependencies/hopscotch-map")

set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_git_submodule("${PROJECT_SOURCE_DIR}/dependencies/rapidjson")

if (UNIX)
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-Ofast -fno-exceptions)
        endif()
endif()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
        set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
        STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/Ot /Ox /Zc:auto /fp:fast /GT /GS- /Oi /GF /utf-8 /D_HAS_EXCEPTIONS=0 /MT /MTd)
        endif()
endif()

include_directories($CACHE{INCLUDE_PATH})

add_library(BlazeFS "$CACHE{SOURCE_PATH}/BlazeFS.cpp")
include_directories("${PROJECT_SOURCE_DIR}/dependencies/rapidjson/include")
include_directories("${PROJECT_SOURCE_DIR}/dependencies/hopscotch-map/include")

if (BUILD_EXAMPLES)
        add_executable(example_write_read_file "$CACHE{EXAMPLES_PATH}/main.cpp")

        check_ipo_supported(RESULT result)
        if(result)
                set_target_properties(example_write_read_file PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        endif()

        target_link_libraries(example_write_read_file PUBLIC BlazeFS)

        target_compile_features(example_write_read_file PUBLIC cxx_std_17)
        set_target_properties(example_write_read_file PROPERTIES CXX_EXTENSIONS OFF)
endif()

if (BUILD_TESTS)
add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
endif()