cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

set(this_project booking)

set(utility ~/workspace/cpp-brown-belt/utility)
include_directories(${utility})

project(${this_project} CXX)

set(headers
  booking.h
  old_booking_providers.h
  old_trip_manager.h
  new_booking_providers.h
  new_trip_manager.h
  )

set(sources
  test.cpp
  )

add_executable(${this_project} ${sources} ${headers})

