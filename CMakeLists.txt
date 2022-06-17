cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME AlgebraDash)
project(${PROJECT_NAME} VERSION 0.0.1)

file(GLOB SHARED_SOURCES src/*.cpp)
file(GLOB_RECURSE MAIN_SOURCES src/main/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SHARED_SOURCES} ${MAIN_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_compile_definitions(${PROJECT_NAME} PRIVATE
  RENDER_THREAD_BATCH_COUNT=10
)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)

set(TRACY_ONLY_LOCALHOST OFF)

# for some reason call stack info collection makes GD not startup
# so it has to be turned off
set(TRACY_NO_CALLSTACK ON)
set(TRACY_CALLSTACK OFF)

add_subdirectory(libraries/tracy)
target_link_libraries(${PROJECT_NAME} Tracy::TracyClient)

find_path(GEODE_SDK_PATH
  NAMES Geode.cmake
  PATHS $ENV{GEODE_SUITE}/sdk /Users/Shared/Geode/suite/sdk /usr/local/geode/sdk
  DOC "Geode SDK path."
  REQUIRED
)
include(${GEODE_SDK_PATH}/Geode.cmake)

setup_geode_mod()
