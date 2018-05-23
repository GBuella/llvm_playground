cmake_minimum_required(VERSION 3.9)
project(tej_playground CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM CONFIG REQUIRED)

# This should not be needed if LLVM would set INTERFACE_INCLUDE_DIRS
include_directories(${LLVM_INCLUDE_DIRS})

add_executable(toy0 src/LLVMEssentials0.cc)
target_link_libraries(toy0 PRIVATE LLVMCore)

add_executable(toy1 src/LLVMEssentials1.cc)
target_link_libraries(toy1 PRIVATE LLVMCore)
