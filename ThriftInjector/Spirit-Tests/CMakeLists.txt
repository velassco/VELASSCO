add_executable( num_list1 num_list1.cpp )
target_include_directories( num_list1 PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( num_list2 num_list2.cpp )
target_include_directories( num_list2 PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( num_list4 num_list4.cpp )
target_include_directories( num_list4 PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( employee employee.cpp )
target_include_directories( employee PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( mesh_header mesh_header.cpp )
target_include_directories( mesh_header PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( parse_header parse_header.cxx )
target_include_directories( parse_header PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( parse_element parse_element.cxx )
target_include_directories( parse_element PRIVATE ${Boost_INCLUDE_DIRS} )

add_executable( parse_result parse_result.cxx )
target_include_directories( parse_result PRIVATE ${Boost_INCLUDE_DIRS} )
