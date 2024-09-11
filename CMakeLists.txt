cmake_minimum_required(VERSION 3.15)
project(mprpc VERSION 1.0)

include_directories(./include)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCRS ON)
set(CMAKE_BUILD_TYPE Release)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
message(STATUS ${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
message(STATUS ${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG -Wall")
endif()

add_subdirectory(src)