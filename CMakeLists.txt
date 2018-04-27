cmake_minimum_required(VERSION 3.5.1)

project(LinuxProgramming)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(OSA1)
add_subdirectory(OSA2)
add_subdirectory(BONUS)
