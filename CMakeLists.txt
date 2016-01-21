cmake_minimum_required(VERSION 2.8)
project(ArchLab3)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -mrtm -Wall")

set(SOURCE_FILES main.cpp helper.cpp helper.h stdafx.h BST.cpp BST.hpp Misc.cpp Misc.hpp TestAndTestAndSetLock.cpp TestAndTestAndSetLock.hpp)
add_executable(ArchLab3 ${SOURCE_FILES})