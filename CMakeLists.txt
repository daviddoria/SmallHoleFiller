cmake_minimum_required(VERSION 2.6)

# Setup install
SET( INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )

PROJECT(SmallHoleFiller)

FIND_PACKAGE(ITK REQUIRED)
INCLUDE( ${USE_ITK_FILE} )

add_executable(SmallHoleFillerExample SmallHoleFillerExample.cpp)
TARGET_LINK_LIBRARIES(SmallHoleFillerExample ${ITK_LIBRARIES})
INSTALL( TARGETS SmallHoleFillerExample RUNTIME DESTINATION ${INSTALL_DIR} )