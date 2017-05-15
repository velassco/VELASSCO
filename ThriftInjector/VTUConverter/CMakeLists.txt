cmake_minimum_required(VERSION 3.1.0)
project (VTUConverter)

#list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../thirdpary/vc14/win64/vtk")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#set(VTK_DIR "${PROJECT_SOURCE_DIR}\\..\\..\\thirdparty\\vc14\\win64\\vtk\\lib\\cmake\\vtk-7.0")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# GLM
#set(GLM_ROOT_DIR "${PROJECT_SOURCE_DIR}\\..\\..\\thirdparty\\src\\glm")
find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

add_executable(VTUConverter 
	main.cpp
	BVHTree.cpp
	BVHTree.h
	AABB.h)
	
target_link_libraries(VTUConverter ${VTK_LIBRARIES})