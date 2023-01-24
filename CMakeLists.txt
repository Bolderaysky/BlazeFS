cmake_minimum_required(VERSION 3.15)

project(vfs_asar VERSION 1.0
        DESCRIPTION "Library that allows you to use virtual filesystem that can be loaded from asar on drive or directly from memory"
        LANGUAGES CXX)

include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_PATH "src" CACHE STRING "Source")
set(INCLUDE_PATH "include" CACHE STRING "Headers")
set(EXAMPLES_PATH "examples" CACHE STRING "Examples for the filesystem")
set(DEFAULT_BUILD_TYPE "Release" CACHE STRING "Define default build type")
option(BUILD_EXAMPLES "Boolean that specifies if it's needed to build examples or not" OFF)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

if (UNIX)
        if (CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-Ofast -fno-exceptions)
        endif()
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

add_library(vfs "$CACHE{SOURCE_PATH}/vfs.cpp")


if (BUILD_EXAMPLES)
        add_executable(example_write_read_file "$CACHE{EXAMPLES_PATH}/main.cpp")

        check_ipo_supported(RESULT result)
        if(result)
                set_target_properties(example_write_read_file PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        endif()

        target_link_libraries(example_write_read_file PUBLIC vfs)

        target_compile_features(example_write_read_file PUBLIC cxx_std_17)
        set_target_properties(example_write_read_file PROPERTIES CXX_EXTENSIONS OFF)
endif()
