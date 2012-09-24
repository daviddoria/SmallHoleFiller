cmake_minimum_required(VERSION 2.6)

# Setup install
SET( INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )

PROJECT(SmallHoleFiller)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")

# Submodules
# Only include Helpers if it is not already available
get_property(FoundMask GLOBAL PROPERTY MaskIncludeDirs SET)
if(NOT FoundMask)
  add_subdirectory(Mask)
endif()

get_property(MaskIncludeDirs GLOBAL PROPERTY MaskIncludeDirs)
set(SmallHoleFiller_include_dirs ${SmallHoleFiller_include_dirs} ${MaskIncludeDirs})
get_property(MaskLibs GLOBAL PROPERTY MaskLibs)
set(SmallHoleFiller_libraries ${SmallHoleFiller_libraries} ${MaskLibs})

# This is so subdirectories can include files in this directory with #include "file.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SmallHoleFiller_include_dirs})

# ITK
FIND_PACKAGE(ITK REQUIRED ITKCommon ITKIOImageBase ITKIOPNG ITKIOMeta
                          ITKImageIntensity ITKImageFeature ITKMathematicalMorphology
                          ITKBinaryMathematicalMorphology ITKDistanceMap)
INCLUDE(${USE_ITK_FILE})
set(SmallHoleFiller_libraries ${SmallHoleFiller_libraries} ${ITK_LIBRARIES})

# VTK
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})
set(SmallHoleFiller_libraries ${SmallHoleFiller_libraries} ${VTK_LIBRARIES})

add_executable(FillSmallHoles FillSmallHoles.cpp)
TARGET_LINK_LIBRARIES(FillSmallHoles ${SmallHoleFiller_libraries})
INSTALL( TARGETS FillSmallHoles RUNTIME DESTINATION ${INSTALL_DIR} )

option(BuildGUI "Build the GUI?" OFF)
if(BuildGUI)
  add_subdirectory(GUI)
endif(BuildGUI)
