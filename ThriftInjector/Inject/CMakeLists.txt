
message( STATUS "THRIFTCPP_INCLUDE_DIRS = ${THRIFTCPP_INCLUDE_DIRS}" )
find_package( Threads REQUIRED )

add_definitions( -DBOOST_LOG_DYN_LINK -DBOOST_SPIRIT_USE_PHOENIX_V3 )

add_executable( ExampleLog ExampleLog.cxx )
target_link_libraries( ExampleLog ${CMAKE_THREAD_LIBS_INIT} 
  ${Boost_LIBRARIES} )

add_executable( ExampleLog_01 ExampleLog_01.cxx Logger.cxx )
target_link_libraries( ExampleLog_01 ${CMAKE_THREAD_LIBS_INIT} 
  ${Boost_LIBRARIES} )

add_executable( gid_inject GidInjectHBase.cxx Logger.cxx GidTypes.cxx ParseAsciiMesh.cxx ParseAsciiResult.cxx BinarySerialization.cxx )
target_include_directories( gid_inject PRIVATE 
  ${Boost_INCLUDE_DIRS}
  ${THRIFTCPP_INCLUDE_DIRS} )
target_link_libraries( gid_inject 
  ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} thriftcpp )

add_executable( check_model CheckHBaseModel.cxx Logger.cxx )
target_include_directories( check_model PRIVATE
  ${Boost_INCLUDE_DIRS}
  ${THRIFTCPP_INCLUDE_DIRS} )
target_link_libraries( check_model
  ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} thriftcpp )

add_executable( TestEndianness TestEndianness.cxx BinarySerialization.cxx )
target_include_directories( TestEndianness PRIVATE 
  ${THRIFTCPP_INCLUDE_DIRS} )
target_link_libraries( TestEndianness
  ${CMAKE_THREAD_LIBS_INIT} ${THRIFT_LIBRARIES} )

