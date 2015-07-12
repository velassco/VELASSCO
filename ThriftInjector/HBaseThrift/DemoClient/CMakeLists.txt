
add_executable( DemoClient DemoClient.cpp )
target_include_directories( DemoClient PRIVATE ${THRIFTCPP_INCLUDE_DIRS} )
target_link_libraries( DemoClient thriftcpp )
