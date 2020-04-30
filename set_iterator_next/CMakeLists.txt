cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ENABLE_EXPORTS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(utility /Users/mnovikov/workspace/cpp-brown-belt/utility)

set(this_project set_iterator_next)
project(${this_project} CXX)

set(headers
	)

set(sources
	set_iterator_next.cpp
	)

add_executable(${this_project} ${sources} ${headers})
target_include_directories(${this_project} PUBLIC ${utility})

