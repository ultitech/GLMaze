# Hunter requires Version > 3.2
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Include more modules, like Find for the FreeImage library
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# Use Hunter for dependency when all libraries are available
#INCLUDE(${CMAKE_MODULE_PATH}/HunterGate.cmake)

#HunterGate(
#    URL "https://github.com/ruslo/hunter/archive/v0.10.9.tar.gz"
#    SHA1 "53b198e364dc7bc8360fc545f798563229bd7e20"
#)

# Mac: Make GLMaze compatible with OS X as early as 10.6
#      has to be set before project is declared
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.6)

# Binary name
set(PROJECT_TARGETBIN "GLMaze")
# Human-readable project name
set(PROJECT_NAME "GLMaze")
# Identifier of project
set(PROJECT_IDENTIFIER "tech.ulti.glmaze")
# Versioning format: http://semver.org
set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)

find_package(Git)
if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            OUTPUT_VARIABLE PROJECT_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# Define project name and language
project(${PROJECT_TARGETBIN} C)

add_subdirectory(src)
