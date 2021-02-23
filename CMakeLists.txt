cmake_minimum_required ( VERSION 3.7 )
project ( grid )

set ( CMAKE_CXX_STANDARD 11 )

set ( SOURCE_FILES main.cpp NestedGroups.h NestedGroups.cpp RunningStats.hpp AcceptanceRejection.h AcceptanceRejection.cpp TOG.h TOG.cpp CompleteBinaryTree.h CompleteBinaryTree.cpp)
add_executable ( grid ${SOURCE_FILES} )
