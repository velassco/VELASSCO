
# Includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/thrift-0.9.2/lib/cpp/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/boost-1_54_0/include/boost-1_54)

# Libraries
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/thrift-0.9.2/lib/cpp/x64/$(ConfigurationName))
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdParty/boost-1_54_0/lib)

# List of Thrift generated sources
set(THRIFT_GEN_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Thrift/gen-cpp)
set(THRIFT_GEN_SOURCES ${THRIFT_GEN_SOURCE_DIR}/QueryManager.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager.h
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager_server.skeleton.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/velassco_constants.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/velassco_constants.h
                       ${THRIFT_GEN_SOURCE_DIR}/velassco_types.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/velassco_types.h)

# List of access library sources
set(ACCESS_LIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ACCESS_LIB_SOURCES ${ACCESS_LIB_SOURCE_DIR}/AccessLib.h
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLib.cpp 
					   ${ACCESS_LIB_SOURCE_DIR}/Client.h 
					   ${ACCESS_LIB_SOURCE_DIR}/Client.cpp
					   ${ACCESS_LIB_SOURCE_DIR}/Server.h
					   ${ACCESS_LIB_SOURCE_DIR}/Server.cpp
					   ${ACCESS_LIB_SOURCE_DIR}/Helpers.h
					   ${ACCESS_LIB_SOURCE_DIR}/Helpers.cpp)

# Group generated sources
source_group(Generated FILES ${THRIFT_GEN_SOURCES})

# Group access library sources
source_group(Sources FILES ${ACCESS_LIB_SOURCES})
					   
# Create the access library.
add_library(AccessLib STATIC ${ACCESS_LIB_SOURCES} ${THRIFT_GEN_SOURCES})

# Access library include directory
target_include_directories(AccessLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Access library link libraries
#target_link_libraries(AccessLib LINK_PRIVATE libthrift libthriftnb)
target_link_libraries(AccessLib LINK_PRIVATE thrift thriftnb event boost_regex boost_date_time)

# Install Access library
install(TARGETS AccessLib DESTINATION ${CMAKE_BINARY_DIR}/Install/${CMAKE_PROJECT_NAME}/lib)
install(FILES AccessLib.h DESTINATION ${CMAKE_BINARY_DIR}/Install/${CMAKE_PROJECT_NAME}/include)
