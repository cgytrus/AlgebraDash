cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(AlgebraDash VERSION 0.0.3)

add_library(${PROJECT_NAME} SHARED
  src/main.cpp
  src/features/optimizations/loading.cpp
  src/features/optimizations/rendering.cpp
  src/profiler/profiler.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
  message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
  message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

setup_geode_mod(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
  include
  libraries/thread-pool
)

target_link_libraries(${PROJECT_NAME} geode-sdk)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  OPTIMIZATIONS
#  CUSTOM_DEBUG
)

set(TRACY_ENABLE OFF)
set(TRACY_ON_DEMAND ON)

set(TRACY_ONLY_LOCALHOST OFF)

set(TRACY_NO_CALLSTACK OFF)
set(TRACY_CALLSTACK OFF)

add_subdirectory(libraries/tracy)
target_include_directories(${PROJECT_NAME} PRIVATE libraries/tracy/public/tracy)
target_link_libraries(${PROJECT_NAME} Tracy::TracyClient)
