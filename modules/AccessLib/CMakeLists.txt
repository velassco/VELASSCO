# Project.
cmake_minimum_required (VERSION 2.8.11)
project (VELaSSCo_AccessLib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Recurse into subdirectories. 
add_subdirectory (AccessLib)
add_subdirectory (Test)