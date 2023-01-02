cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(AlgebraDash)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# this is so stupid
set(CMAKE_SIZEOF_VOID_P 4)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  # ensure 32 bit on clang
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target i386-pc-windows-msvc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target i386-pc-windows-msvc")
  add_definitions("--target=i386-pc-windows-msvc")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
  include
  libraries/minhook/include
  libraries/gd.h/include
  libraries/gd.h
  libraries/thread-pool
  libraries/hackpro/include
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/cocos-headers)
add_subdirectory(libraries/mat-dash)

target_link_libraries(${PROJECT_NAME} minhook cocos2d mat-dash)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  OPTIMIZATIONS
#  CUSTOM_DEBUG
)

set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)

set(TRACY_ONLY_LOCALHOST OFF)

set(TRACY_NO_CALLSTACK OFF)
set(TRACY_CALLSTACK OFF)

add_subdirectory(libraries/tracy)
target_include_directories(${PROJECT_NAME} PRIVATE libraries/tracy/public/tracy)
target_link_libraries(${PROJECT_NAME} Tracy::TracyClient)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/hackpro/bin/hackpro.lib)
target_link_options(${PROJECT_NAME} PRIVATE "/DELAYLOAD:hackpro.dll")
