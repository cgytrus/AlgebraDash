cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_NAME AlgebraDash)
project(${PROJECT_NAME})

file(GLOB SHARED_SOURCES src/*.cpp)
file(GLOB_RECURSE MAIN_SOURCES src/main/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SHARED_SOURCES} ${MAIN_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
  libraries/minhook/include
  libraries/gd.h/include
  libraries/gd.h
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/cocos-headers)
target_link_libraries(${PROJECT_NAME} minhook cocos2d)

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
