# add_executable(consumer friendservicecaller.cc ../friend.pb.cc)
add_executable(consumer userservicecaller.cc ../user.pb.cc)
target_link_libraries(consumer PRIVATE zookeeper_mt mprpc mymuduo protobuf)
set_target_properties(consumer PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                          ${CMAKE_SOURCE_DIR}/bin)
