
add_library( thriftcpp 
  gen-cpp/Hbase.cpp gen-cpp/Hbase_types.cpp gen-cpp/Hbase_constants.cpp )

target_include_directories( thriftcpp PUBLIC
  ${THRIFT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

target_link_libraries( thriftcpp ${THRIFT_LIBRARIES} )

set( THRIFTCPP_INCLUDE_DIRS 
  "${THRIFT_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp" 
  CACHE INTERNAL "" FORCE )

add_subdirectory( DemoClient )
