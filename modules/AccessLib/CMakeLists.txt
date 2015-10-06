# Project.
cmake_minimum_required (VERSION 2.8.11)
project (VELaSSCo_AccessLib)

set( SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" )

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CXX_4_4_STD_COMPILE_FLAG "-std=c++0x -fPIC")
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CXX_4_4_STD_COMPILE_FLAG}" )
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

if( NOT MSVC )
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
  find_package( Thrift REQUIRED )
  message( STATUS "THRIFT_LIBRARIES = ${THRIFT_LIBRARIES}" )
  message( STATUS "THRIFT_INCLUDE_DIR = ${THRIFT_INCLUDE_DIR}" )

  # http://stackoverflow.com/questions/9948375/cmake-find-package-succeeds-but-returns-wrong-path
  # use -DBoost_NO_BOOST_CMAKE=ON to overcome the problem above described
  # Added version 1.5 to get version 1.54 on windows and 1.58 on acuario/pez
  # instead of old 1.41 instaled on /usr/lib64 which shows the problem described in the above link
  # and 'make' tries to link with '/usr/lib64/lib64/libboost_regex-mt.so.5' and make it.
  find_package( Boost 1.5 REQUIRED COMPONENTS regex date_time system)
else()
  set( THRIFT_INCLUDE_DIR "${SOURCE_ROOT}/3rdParty/thrift-0.9.2/lib/cpp/src" )
  set( Boost_INCLUDE_DIRS "${SOURCE_ROOT}/3rdParty/boost-1_54_0/include/boost-1_54" )
  set( BOOST_LIBRARY_DIR "${SOURCE_ROOT}/3rdParty/boost-1_54_0/lib" )
  set( THRIFT_LIBRARY_DIR "${SOURCE_ROOT}/3rdParty/thrift-0.9.2/lib/cpp/x64/${CMAKE_BUILD_TYPE}" )
  message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
  set( THRIFT_LIBRARIES libthrift libthriftnb )
  set( Boost_LIBRARIES "" )
  set( LIBEVENT_LIBRARY "" )
endif( )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Recurse into subdirectories. 
add_subdirectory (AccessLib)
add_subdirectory (Test)

