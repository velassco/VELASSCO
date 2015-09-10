# Project.
cmake_minimum_required (VERSION 2.8.11)
project (VELaSSCo_AccessLib)

# specific flags for g++ 4.4.X
SET(CXX_4_4_STD_COMPILE_FLAG "-std=c++0x")
SET(GXX_4_4_STD_LINK_FLAG "-std=c++0x -L/usr/local/lib")

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CXX_4_4_STD_COMPILE_FLAG}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GXX_4_4_STD_LINK_FLAG}" )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Recurse into subdirectories. 
add_subdirectory (AccessLib)
add_subdirectory (Test)