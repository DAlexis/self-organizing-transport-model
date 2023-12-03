
#if(POLICY CMP0020)
#  cmake_policy(SET CMP0020 NEW)
#endif()

PROJECT(sotm-gui)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(${VTK_VERSION} VERSION_GREATER "6" AND ${VTK_QT_VERSION} VERSION_GREATER "4")
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
  SET(CMAKE_AUTOUIC ON)
  find_package(Qt5Widgets REQUIRED QUIET)
else()
  message(FATAL_ERROR "Yor VTK_VERSION < 6 or VTK_QT_VERSION < 4, build system was not tested for this configuration")
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
endif()

set(UI_FILES
    ${PROJECT_SOURCE_DIR}/ui/RenderWindowUIMultipleInheritance.ui
)

# Headers wirth Q_OBJECTs
set(EXE_QT_HPP
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/visualizer-ui-window.hpp
)

# Headers without Q_OBJECT  
set(EXE_HPP
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/gui-internal.hpp
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/async-iteration.hpp
    ${PROJECT_SOURCE_DIR}/sotm-gui/gui.hpp
)

# All source files
set(EXE_SOURCE
    ${PROJECT_SOURCE_DIR}/source/visualizer-ui-window.cpp
    ${PROJECT_SOURCE_DIR}/source/gui.cpp
    ${PROJECT_SOURCE_DIR}/source/async-iteration.cpp
)

if(${VTK_VERSION} VERSION_GREATER "6" AND ${VTK_QT_VERSION} VERSION_GREATER "4")
  # Create headers from .ui files with moc
  qt5_wrap_ui(AUTOGEN_UI_SOURCE ${UI_FILES} )
  
  # CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.  
  add_library(${PROJECT_NAME} ${EXE_SOURCE} ${AUTOGEN_UI_SOURCE} ${EXE_HPP} ${EXE_QT_HPP})

  target_include_directories(${PROJECT_NAME} PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${sotm_INCLUDE_DIRS}
      ${PROJECT_SOURCE_DIR}
      ${Qt5Widgets_INCLUDE_DIRS}
      ${Qt5Core_INCLUDE_DIRS}
  )
  
  target_link_libraries(${PROJECT_NAME} PUBLIC ${VTK_LIBRARIES} sotm)
  
else()
    message(FATAL_ERROR "Yor VTK_VERSION < 6 or VTK_QT_VERSION < 4, cannot build")
endif()

