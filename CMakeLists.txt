cmake_minimum_required(VERSION 3.5.1)

project(LinuxProgramming)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
set(BONUS_TASK3_MAX_CONNECTIONS 10 CACHE INT "Maximum number of connections for bonus task 3")

add_subdirectory(OSA1)
add_subdirectory(OSA2)
add_subdirectory(BONUS)
