cmake_minimum_required(VERSION 3.15)
project(example)

include_directories(. ../include)

link_directories(../lib)

add_subdirectory(./callee ./caller)
