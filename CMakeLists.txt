cmake_minimum_required(VERSION 2.6)
PROJECT(clanlib_racer)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

MESSAGE("Configuring build type: ${CMAKE_BUILD_TYPE}")

SET(VERBOSE false)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Modules ${CMAKE_ROOT}/Modules)

ADD_SUBDIRECTORY(src)
