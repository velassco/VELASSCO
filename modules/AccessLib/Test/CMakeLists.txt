if( MSVC )
  link_directories( ${THRIFT_LIBRARY_DIR} ${BOOST_LIBRARY_DIR} )
endif()

# Add Test executables
add_executable(Test_Client ${SOURCE_ROOT}/Test/Test_Client.cpp)
add_executable(Test_Server ${SOURCE_ROOT}/Test/Test_Server.cpp)
add_executable(lrvol_appr_client ${SOURCE_ROOT}/Test/lrvol_appr_client.cpp)

# Link access libraries
target_link_libraries(Test_Client LINK_PUBLIC AccessLib -lz)
target_link_libraries(Test_Server LINK_PUBLIC AccessLib -lz)
target_link_libraries(lrvol_appr_client LINK_PUBLIC AccessLib -lz)

# Group targets
set_target_properties(Test_Client PROPERTIES FOLDER "Testing")
set_target_properties(Test_Server PROPERTIES FOLDER "Testing")
set_target_properties(lrvol_appr_client PROPERTIES FOLDER "Testing")
