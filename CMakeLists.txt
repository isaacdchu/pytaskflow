cmake_minimum_required(VERSION 3.15...3.27)
project(nano_test)

# Require C++20 for building modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_VERSION VERSION_LESS 3.18)
    set(DEV_MODULE Development)
else()
    set(DEV_MODULE Development.Module)
endif()

find_package(Python 3.13.7 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Detect the installed nanobind package and import it into CMake
execute_process(
    COMMAND python -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

nanobind_add_module(
    pytaskflow
    FREE_THREADED
    pytaskflow.cpp
)

# If a vendored taskflow copy exists under third_party, add it to the
# include path so code can use #include <taskflow/taskflow.hpp> without
# copying headers into the virtualenv.
if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/taskflow")
    # Add the parent third_party directory so includes like
    #   #include <taskflow/taskflow.hpp>
    # resolve to third_party/taskflow/taskflow.hpp
    target_include_directories(pytaskflow PRIVATE "${CMAKE_SOURCE_DIR}/third_party")
    message(STATUS "Using vendored Taskflow headers at ${CMAKE_SOURCE_DIR}/third_party/taskflow")
endif()