# aux_source_directory(. CALLER_LIST)

# add_executable(provider ${CALLER_LIST} ../User.pb.cc)
# target_link_libraries(provider PRIVATE protobuf)
# set_target_properties(provider PROPERTIES RUNTIME_OUTPUT_DIRECTORY
#                                           ${CMAKE_SOURCE_DIR}/bin)