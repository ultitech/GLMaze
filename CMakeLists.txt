cmake_minimum_required(VERSION 3.2 FATAL_ERROR)


SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# Use Hunter for dependency when all libraries are available 
#INCLUDE(${CMAKE_MODULE_PATH}/HunterGate.cmake)

#HunterGate(
#    URL "https://github.com/ruslo/hunter/archive/v0.10.9.tar.gz"
#    SHA1 "53b198e364dc7bc8360fc545f798563229bd7e20"
#)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.6)

project(GLMaze C)

add_subdirectory(src)
