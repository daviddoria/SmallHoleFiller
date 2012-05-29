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

add_executable(FillSmallHolesAllComponents FillSmallHolesAllComponents.cpp)
TARGET_LINK_LIBRARIES(FillSmallHolesAllComponents ${ITK_LIBRARIES} ITKHelpers Mask)
INSTALL( TARGETS FillSmallHolesAllComponents RUNTIME DESTINATION ${INSTALL_DIR} )

add_executable(FillSmallHolesRGB FillSmallHolesRGB.cpp)
TARGET_LINK_LIBRARIES(FillSmallHolesRGB ${ITK_LIBRARIES} ITKHelpers Mask)
INSTALL( TARGETS FillSmallHolesRGB RUNTIME DESTINATION ${INSTALL_DIR} )

add_executable(FillSmallHolesScalar FillSmallHolesScalar.cpp)
TARGET_LINK_LIBRARIES(FillSmallHolesScalar ${ITK_LIBRARIES} ITKHelpers Mask)
INSTALL( TARGETS FillSmallHolesScalar RUNTIME DESTINATION ${INSTALL_DIR} )

#add_subdirectory(GUI)
