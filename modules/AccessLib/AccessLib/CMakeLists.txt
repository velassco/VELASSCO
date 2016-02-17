
# Includes
include_directories( ${THRIFT_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIRS} )

# List of Thrift generated sources
# this is not the correct folder :
# set(THRIFT_GEN_SOURCE_DIR ${SOURCE_ROOT}/Thrift/gen-cpp)
set(THRIFT_GEN_SOURCE_DIR ${SOURCE_ROOT}/../thrift/QueryManager/gen-cpp)
set(THRIFT_GEN_SOURCES ${THRIFT_GEN_SOURCE_DIR}/QueryManager.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager.h
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager_server.skeleton.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager_constants.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager_constants.h
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager_types.cpp
                       ${THRIFT_GEN_SOURCE_DIR}/QueryManager_types.h)

# List of access library sources
set(ACCESS_LIB_SOURCE_DIR ${SOURCE_ROOT}/AccessLib)
set(ACCESS_LIB_SOURCES ${ACCESS_LIB_SOURCE_DIR}/AccessLib.h
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLibCommon.h
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLibCommon.cpp
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLibSQ.cpp 
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLibDRQ.cpp 
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLibRAQ.cpp 
                       ${ACCESS_LIB_SOURCE_DIR}/AccessLibMQ.cpp
		       ${ACCESS_LIB_SOURCE_DIR}/Client.h 
		       ${ACCESS_LIB_SOURCE_DIR}/Client.cpp
		       ${ACCESS_LIB_SOURCE_DIR}/DemoServer.h
		       ${ACCESS_LIB_SOURCE_DIR}/DemoServer.cpp
		       ${ACCESS_LIB_SOURCE_DIR}/Helpers.h
		       ${ACCESS_LIB_SOURCE_DIR}/Helpers.cpp
		       ${ACCESS_LIB_SOURCE_DIR}/base64.cpp)

# Group generated sources
source_group(Generated FILES ${THRIFT_GEN_SOURCES})

# Group access library sources
source_group(Sources FILES ${ACCESS_LIB_SOURCES})
					   
# Create the access library.
add_library(AccessLib STATIC ${ACCESS_LIB_SOURCES} ${THRIFT_GEN_SOURCES})

# Access library include directory
target_include_directories(AccessLib PUBLIC ${ACCESS_LIB_SOURCE_DIR})

target_link_libraries( AccessLib LINK_PUBLIC ${THRIFT_LIBRARIES}
  ${Boost_LIBRARIES} ${LIBEVENT_LIBRARY} )

# Install Access library
install(TARGETS AccessLib DESTINATION ${CMAKE_BINARY_DIR}/Install/${CMAKE_PROJECT_NAME}/lib)
install(FILES AccessLib.h DESTINATION ${CMAKE_BINARY_DIR}/Install/${CMAKE_PROJECT_NAME}/include)
