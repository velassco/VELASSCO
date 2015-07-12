add_executable( TestThriftSerializer TestThriftSerializer.cxx )
target_link_libraries( TestThriftSerializer ${THRIFT_LIBRARIES} )
target_include_directories( TestThriftSerializer PRIVATE
  ${THRIFT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})