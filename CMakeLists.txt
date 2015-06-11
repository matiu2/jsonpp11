project(JSONpp11)

cmake_minimum_required(VERSION 2.8)

add_definitions( -std=c++11 -Wall -Wextra )

option(BUILD_TESTS "Build the test suite and download the bandit testing framework" OFF)

if (${BUILD_TESTS})
    include(cmake/get_bandit.cmake)
    enable_testing()
    include_directories(${BANDIT_INCLUDE_DIR})
endif()

option(LOCATIONS "Provide a row+col location when a json parsing error is hit" true)
if(NOT LOCATIONS)
  add_definitions(-DNO_LOCATIONS)
endif()

# All tests go here

add_subdirectory(src)

# This libary has no code, it's only made to help with cmake dependency resulotion
add_library(jsonpp11 empty.cpp)
