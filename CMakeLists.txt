cmake_minimum_required(VERSION 3.3)
project(ArchLab3)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp BST.cpp BST.hpp TestAndTestAndSetLock.cpp TestAndTestAndSetLock.hpp helper.h stdafx.h helper.cpp HardwareLockElision.cpp HardwareLockElision.hpp)
add_executable(ArchLab3 ${SOURCE_FILES})