# Project.
cmake_minimum_required (VERSION 2.8.11)
project (VELaSSCo_AccessLib)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CXX_4_4_STD_COMPILE_FLAG "-std=c++0x")
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CXX_4_4_STD_COMPILE_FLAG}" )
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

set( CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH} )

find_library( LIBEVENT_LIBRARY
  NAMES event )

if ( LIBEVENT_LIBRARY )
  message( STATUS "libevent found: ${LIBEVENT_LIBRARY}" )
else()
  message( FATAL "could not find libevent" )
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package( Thrift REQUIRED )
message( STATUS "THRIFT_LIBRARIES = ${THRIFT_LIBRARIES}" )
message( STATUS "THRIFT_INCLUDE_DIR = ${THRIFT_INCLUDE_DIR}" )

# http://stackoverflow.com/questions/9948375/cmake-find-package-succeeds-but-returns-wrong-path
# use -DBoost_NO_BOOST_CMAKE=ON to overcome the problem above described
find_package( Boost REQUIRED COMPONENTS 
  regex date_time )

set( SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." )
# Recurse into subdirectories. 
add_subdirectory (AccessLib)
add_subdirectory (Test)

