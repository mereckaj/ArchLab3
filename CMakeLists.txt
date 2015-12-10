cmake_minimum_required(VERSION 2.8)
project(ArchLab3)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall")
#set( CMAKE_VERBOSE_MAKEFILE on)
set(SOURCE_FILES main.cpp BST.cpp BST.hpp TestAndTestAndSetLock.cpp TestAndTestAndSetLock.hpp helper.h stdafx.h helper.cpp HardwareLockElision.cpp HardwareLockElision.hpp)
add_executable(ArchLab3 ${SOURCE_FILES})