cmake_minimum_required(VERSION 3.15)
project(mprpc VERSION 1.0)

include_directories(./include)
add_subdirectory(src)
set(CMAKE_BUILD_TYPE Debug)