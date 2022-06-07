cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME AlgebraDash)
set(PROFILER_NAME AlgebraDashProfiler)
set(PROFILER_NO_MEMORY_NAME AlgebraDashProfilerNoMemory)
project(${PROJECT_NAME})

file(GLOB SHARED_SOURCES src/*.cpp)
file(GLOB_RECURSE MAIN_SOURCES src/main/*.cpp)
file(GLOB_RECURSE PROFILER_SOURCES src/profiler/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SHARED_SOURCES} ${MAIN_SOURCES})
add_library(${PROFILER_NAME} SHARED ${SHARED_SOURCES} ${PROFILER_SOURCES})
add_library(${PROFILER_NO_MEMORY_NAME} SHARED ${SHARED_SOURCES} ${PROFILER_SOURCES})

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

target_include_directories(${PROFILER_NAME} PRIVATE
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

target_include_directories(${PROFILER_NO_MEMORY_NAME} PRIVATE
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
target_link_libraries(${PROFILER_NAME} minhook)
target_link_libraries(${PROFILER_NO_MEMORY_NAME} minhook)

target_compile_definitions(${PROFILER_NAME} PRIVATE
  PROFILE_MEMORY
  PROFILER_MEMORY_TRACKER # makes sure invalid alloc/free calls aren't sent to tracy
  MEMORY_TRACKER_MESSAGES # whether to send tracy messages when an invalid alloc/free is called or not
)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)

set(TRACY_ONLY_LOCALHOST OFF)

# for some reason call stack info collection makes GD not startup
# so it has to be turned off
set(TRACY_NO_CALLSTACK ON)
set(TRACY_CALLSTACK OFF)

add_subdirectory(libraries/tracy)
target_link_libraries(${PROFILER_NAME} Tracy::TracyClient)
target_link_libraries(${PROFILER_NO_MEMORY_NAME} Tracy::TracyClient)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/extensions/libExtensions.lib)

# not sure if i need these in the profiler
target_link_libraries(${PROFILER_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib)
target_link_libraries(${PROFILER_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/extensions/libExtensions.lib)

target_link_libraries(${PROFILER_NO_MEMORY_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib)
target_link_libraries(${PROFILER_NO_MEMORY_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/extensions/libExtensions.lib)
