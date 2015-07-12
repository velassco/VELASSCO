cmake_minimum_required( VERSION 2.8 )

project( ThriftInjector )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  ${CMAKE_MODULE_PATH} )

find_package( Thrift REQUIRED )

message( STATUS "THRIFT_LIBRARIES = ${THRIFT_LIBRARIES}" )
message( STATUS "THRIFT_INCLUDE_DIR = ${THRIFT_INCLUDE_DIR}" )

find_package( Boost REQUIRED COMPONENTS 
  system thread filesystem log program_options )

add_subdirectory( HBaseThrift )
add_subdirectory( Inject )
add_subdirectory( Spirit-Tests )
add_subdirectory( SandBox )
