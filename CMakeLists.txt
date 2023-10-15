cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(AlgebraDash VERSION 0.0.3)

add_library(${PROJECT_NAME} SHARED
  src/shared_pool.cpp
  #src/debug/profiler.cpp
  #src/debug/stats.cpp
  src/features/improvements/anti_aliasing.cpp
  src/features/improvements/fast_alt_tab.cpp
  src/features/improvements/force_rgba8888.cpp
  src/features/improvements/main_loop_rewrite.cpp
  src/features/improvements/smooth_particles.cpp
  src/features/optimizations/loading/qoi_cache.cpp
  src/features/optimizations/loading/parallel_loading.cpp
  src/features/optimizations/processing/move_actions.cpp
  src/features/optimizations/processing/fast_pickup.cpp
  src/features/optimizations/rendering/fast_batch_sort.cpp
  #src/features/optimizations/rendering/opengl_update.cpp
  src/features/optimizations/rendering/parallel_batch.cpp
  src/features/optimizations/rendering/update_visibility_rewrite.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
  message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
  message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} $ENV{GEODE_SDK}/build)

setup_geode_mod(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} geode-sdk)
if(WIN32)
  target_link_libraries(${PROJECT_NAME} opengl32)
  #target_compile_options(${PROJECT_NAME} PRIVATE "/fp:fast" "/arch:AVX512")
elseif(APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL -framework Cocoa")
elseif(ANDROID)
  target_link_libraries(${PROJECT_NAME} GLESv2)
endif()

CPMAddPackage("gh:cgytrus/qoixx#6fe8c5a")
target_include_directories(${PROJECT_NAME} PRIVATE ${qoixx_SOURCE_DIR}/include)

CPMAddPackage("gh:bshoshany/thread-pool#cabb3df")
target_include_directories(${PROJECT_NAME} PRIVATE ${thread-pool_SOURCE_DIR}/include)

set(TRACY_ENABLE OFF CACHE BOOL "" FORCE)
set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)

set(TRACY_ONLY_LOCALHOST OFF CACHE BOOL "" FORCE)

set(TRACY_NO_CALLSTACK OFF CACHE BOOL "" FORCE)
set(TRACY_CALLSTACK OFF CACHE BOOL "" FORCE)

CPMAddPackage("gh:wolfpld/tracy#93537df")
target_include_directories(${PROJECT_NAME} PRIVATE ${Tracy_SOURCE_DIR}/public/tracy)
target_link_libraries(${PROJECT_NAME} Tracy::TracyClient)
