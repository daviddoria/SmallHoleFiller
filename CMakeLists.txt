cmake_minimum_required(VERSION 2.6)

# Setup install
SET( INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )

PROJECT(SmallHoleFiller)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")

# Submodules
add_subdirectory(Mask)

# This is so subdirectories can include files in this directory with #include "file.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE( ${USE_ITK_FILE} )

add_executable(FillSmallHoles FillSmallHoles.cpp)
TARGET_LINK_LIBRARIES(FillSmallHoles ${ITK_LIBRARIES} ITKHelpers Mask)
INSTALL( TARGETS FillSmallHoles RUNTIME DESTINATION ${INSTALL_DIR} )

SET(BuildGUI ON CACHE BOOL "Build the GUI" FORCE)
if(BuildGUI)
  add_subdirectory(GUI)
endif(BuildGUI)
