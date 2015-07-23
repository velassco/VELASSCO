
# Required 3rd party libraries (for AccessLib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/thrift-0.9.2/lib/cpp/x64/$(ConfigurationName))
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/boost-1_54_0/lib)

# Add Test executables
add_executable(Test_Client Test_Client.cpp)
add_executable(Test_Server Test_Server.cpp)

# Link access libraries
target_link_libraries(Test_Client LINK_PUBLIC AccessLib)
target_link_libraries(Test_Server LINK_PUBLIC AccessLib)

# Group targets
set_target_properties(Test_Client PROPERTIES FOLDER "Testing")
set_target_properties(Test_Server PROPERTIES FOLDER "Testing")
