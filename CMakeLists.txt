cmake_minimum_required(VERSION 2.8)

project(algorithms)

set(CXX_FLAGS "-std=c++14 -Wall")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CXX_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CXX_FLAGS} -O2 -DNDEBUG")

add_subdirectory(data_structure)
add_subdirectory(sort)
add_subdirectory(string)
add_subdirectory(numerical)
add_subdirectory(extra)
