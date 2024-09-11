cmake_minimum_required(VERSION 3.15)
project(example VERSION 1.0)

include_directories(. ../include)
link_directories(/usr/lib/mymuduo /usr/lib/mprpc)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(callee)
add_subdirectory(caller)
