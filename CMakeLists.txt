cmake_minimum_required(VERSION 2.6)

# Setup install
SET( INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )

PROJECT(SmallHoleFiller)

add_custom_target(SmallHoleFillerSources SOURCES
SmallHoleFiller.h
SmallHoleFiller.hpp)

# Enable c++11
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++11")
endif()

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

# Allow this project to be detected and used as a submodule
set(SmallHoleFiller_include_dirs ${SmallHoleFiller_include_dirs} ${CMAKE_CURRENT_SOURCE_DIR})
set_property(GLOBAL PROPERTY SmallHoleFillerIncludeDirs ${SmallHoleFiller_include_dirs})
set_property(GLOBAL PROPERTY SmallHoleFillerLibs ${SmallHoleFiller_libraries})

# Create an executable
option(BUILD_SmallHoleFiller "Build the SmallHoleFiller executable?" ON)
if(BUILD_SmallHoleFiller)
    add_executable(FillSmallHoles FillSmallHoles.cpp)
    TARGET_LINK_LIBRARIES(FillSmallHoles ${SmallHoleFiller_libraries})
    INSTALL( TARGETS FillSmallHoles RUNTIME DESTINATION ${INSTALL_DIR} )
endif()

# Build the GUI if requested
option(BuildGUI "Build the GUI?" OFF)
if(BuildGUI)
  add_subdirectory(GUI)
endif(BuildGUI)

# Display where this code was used from (if it is used as a submodule, there may be multiple instances of this submodule in the project, only the first of which is used)
option(SmallHoleFiller_ShowSubmoduleLocation "Show the path from which SmallHoleFiller was used?" OFF)
if(SmallHoleFiller_ShowSubmoduleLocation)
  message("SmallHoleFiller used from ${CMAKE_CURRENT_SOURCE_DIR}")
endif(SmallHoleFiller_ShowSubmoduleLocation)
