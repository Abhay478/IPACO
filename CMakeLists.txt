cmake_minimum_required(VERSION 3.5)
project(MY_CP)

find_package(LLVM REQUIRED CONFIG PATHS /home/abhay/Code/LLVM/llvm-project/build/lib/cmake/llvm)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")
add_subdirectory(my_cp)
add_subdirectory(deep_cp)