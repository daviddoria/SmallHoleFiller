FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

# Submodules
add_subdirectory(ITKQtHelpers)

# Let Qt find it's MOCed files
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_UI(SmallHoleFillerWidgetUISrcs SmallHoleFillerWidget.ui)
QT4_WRAP_CPP(SmallHoleFillerWidgetMOCSrcs SmallHoleFillerWidget.h)

ADD_EXECUTABLE(SmallHoleFillerInteractive SmallHoleFillerWidget.cpp SmallHoleFillerInteractive.cpp
                ${SmallHoleFillerWidgetUISrcs} ${SmallHoleFillerWidgetMOCSrcs})
TARGET_LINK_LIBRARIES(SmallHoleFillerInteractive ${ITK_LIBRARIES} ${QT_LIBRARIES}
Mask)
