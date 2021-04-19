cmake_minimum_required ( VERSION 3.7 )
project ( grid )

set ( CMAKE_CXX_STANDARD 11 )

set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O2")
set(CMAKE_VERBOSE_MAKEFILE ON)

set ( SOURCE_FILES main.cpp CascadeOfGroups.h CascadeOfGroups.cpp RunningStats.hpp AcceptanceRejection.h
        AcceptanceRejection.cpp TOG.h TOG.cpp CompleteBinaryTree.h CompleteBinaryTree.cpp TwoLevelAR.cpp TwoLevelAR.h
        TwoLevelARB.cpp TwoLevelARB.h)
add_executable ( grid ${SOURCE_FILES} )
