cmake_minimum_required(VERSION 2.6)

# Setup install
SET( INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )

PROJECT(SmallHoleFiller)

# This is so subdirectories can include files in this directory with #include "file.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE( ${USE_ITK_FILE} )

add_executable(FillSmallHolesAllComponents FillSmallHolesAllComponents.cpp Mask.cpp)
TARGET_LINK_LIBRARIES(FillSmallHolesAllComponents ${ITK_LIBRARIES})
INSTALL( TARGETS FillSmallHolesAllComponents RUNTIME DESTINATION ${INSTALL_DIR} )

add_executable(FillSmallHolesRGB FillSmallHolesRGB.cpp Mask.cpp)
TARGET_LINK_LIBRARIES(FillSmallHolesRGB ${ITK_LIBRARIES})
INSTALL( TARGETS FillSmallHolesRGB RUNTIME DESTINATION ${INSTALL_DIR} )

add_executable(FillSmallHolesScalar FillSmallHolesScalar.cpp Mask.cpp)
TARGET_LINK_LIBRARIES(FillSmallHolesScalar ${ITK_LIBRARIES})
INSTALL( TARGETS FillSmallHolesScalar RUNTIME DESTINATION ${INSTALL_DIR} )

#add_subdirectory(GUI)
