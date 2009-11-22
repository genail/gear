cmake_minimum_required(VERSION 2.6)

PROJECT(gear)

# determine build type
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# build options
OPTION(USE_GL2 "Set to OFF to build using OpenGL 1.x instead of 2.0" ON)
OPTION(RACE_SCENE_ONLY "Set to ON to display only race scene" OFF)

MESSAGE("Configuring build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS)
MESSAGE(STATUS "------------------------------------")
MESSAGE(STATUS "Build")
MESSAGE(STATUS "USE_GL2 = ${USE_GL2}")
MESSAGE(STATUS)
MESSAGE(STATUS "Devel")
MESSAGE(STATUS "RACE_SCENE_ONLY = ${RACE_SCENE_ONLY}")
MESSAGE(STATUS)
MESSAGE(STATUS "Change a value with cmake -D<Variable>=<Value> .")
MESSAGE(STATUS "Or run interactive mode with make edit_cache")
MESSAGE(STATUS "------------------------------------")
MESSAGE(STATUS)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Modules ${CMAKE_ROOT}/Modules)

ADD_SUBDIRECTORY(src)
