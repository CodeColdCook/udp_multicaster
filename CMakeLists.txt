cmake_minimum_required(VERSION 3.5)

project(test_udp
  VERSION 1.0
)

# =============#
# Dependencies #
# =============#

# ----------------------#
# Third-party libraries #
# ----------------------#

find_package(GTest REQUIRED) 
# =========#
# Settings #
# =========#

# install path
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

# shared libraries
if(NOT DEFINED BUILD_SHARED_LIBS)
  message(STATUS "${PROJECT_NAME}: Building dynamically-linked binaries")
  option(BUILD_SHARED_LIBS "Build dynamically-linked binaries" ON)
  set(BUILD_SHARED_LIBS ON)
endif()

# build type
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  message(STATUS "${PROJECT_NAME}: Defaulting build type to RelWithDebInfo")
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# global compilations
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions(-O2)

# useful macros
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake)

# ##############################################################################
# Build #
# ##############################################################################

# ==============#
# Build targets #
# ==============#

include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# ---------------#
# Library target #
# ---------------#

# find all source files
file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.cpp
     ${PROJECT_SOURCE_DIR}/src/*/*.cpp)

# common library
zetton_cc_library(
  NAME
  ${PROJECT_NAME}
  SRCS
  ${src_files}
  INCLUDES
  ${GTEST_INCLUDE_DIRS}
  DEPS
  ${GTEST_BOTH_LIBRARIES}
  pthread
  )

add_simple_apps() 

# unit test
# tests
option(BUILD_WITH_TESTS "Build project with tests" ON)
if (BUILD_WITH_TESTS)
   add_tests_in_dir(broadcast)
   add_tests_in_dir(mtlticast)
endif (BUILD_WITH_TESTS)