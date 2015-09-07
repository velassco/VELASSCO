cmake_minimum_required( VERSION 2.8 )

project( IntOccurrenceMapReduce )

find_package(Java REQUIRED)
include(UseJava)

add_subdirectory( src )

