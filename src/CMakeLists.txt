aux_source_directory(. SRC_LIST)
link_directories(/usr/lib/mymuduo)
add_library(mprpc SHARED ${SRC_LIST})
set_target_properties(mprpc PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                       ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(mprpc PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(mprpc PRIVATE mymuduo zookeeper_mt)
