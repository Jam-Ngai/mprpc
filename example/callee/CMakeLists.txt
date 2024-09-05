aux_source_directory(. CALLEE_LIST)

add_executable(consumer ${CALLEE_LIST} ../user.pb.cc)
target_link_libraries(consumer PRIVATE mprpc protobuf)
set_target_properties(consumer PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                          ${CMAKE_SOURCE_DIR}/bin)
