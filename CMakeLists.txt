cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME AlgebraDash)
project(${PROJECT_NAME})

file(GLOB SHARED_SOURCES src/*.cpp)
file(GLOB_RECURSE MAIN_SOURCES src/main/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SHARED_SOURCES} ${MAIN_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
  libraries/minhook/include
  libraries/gd.h/include
  libraries/gd.h
  libraries/cocos-headers/
  libraries/cocos-headers/extensions
  libraries/cocos-headers/cocos2dx
  libraries/cocos-headers/cocos2dx/include
  libraries/cocos-headers/cocos2dx/kazmath/include
  libraries/cocos-headers/cocos2dx/platform/win32
  libraries/cocos-headers/cocos2dx/platform/third_party/win32
  libraries/cocos-headers/cocos2dx/platform/third_party/win32/OGLES
)

add_subdirectory(libraries/minhook)
target_link_libraries(${PROJECT_NAME} minhook)

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

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/extensions/libExtensions.lib)
