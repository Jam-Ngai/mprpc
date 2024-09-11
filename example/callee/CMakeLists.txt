# add_executable(provider friendservice.cc ../friend.pb.cc)
add_executable(provider userservice.cc ../user.pb.cc)
target_link_libraries(provider PRIVATE zookeeper_mt mprpc mymuduo protobuf)
set_target_properties(provider PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                          ${CMAKE_SOURCE_DIR}/bin)
