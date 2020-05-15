cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -Wextra -pedantic -Werror)

set(this_project cache)

set(utility ~/workspace/cpp-brown-belt/utility)
include_directories(${utility})

project(${this_project} CXX)

set(headers
  Common.h
  )

set(sources
  Solution.cpp
  ${this_project}.cpp
  )

add_executable(${this_project} ${sources} ${headers})

