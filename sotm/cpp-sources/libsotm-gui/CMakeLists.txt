
cmake_minimum_required(VERSION 2.8)
 
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

PROJECT(sotm-gui)

set(EXE_NAME ${PROJECT_NAME})
set(LIB_NAME ${PROJECT_NAME})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
  find_package(Qt5Widgets REQUIRED QUIET)
else()
  message(WARNING "Yor VTK_VERSION < 6 or VTK_QT_VERSION < 4, build system was not tested for this configuration")
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
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/frame-maker.hpp
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/gui-internal.hpp
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/animation-maker.hpp
    ${PROJECT_SOURCE_DIR}/sotm-gui-internal/async-iteration.hpp
    ${PROJECT_SOURCE_DIR}/sotm-gui/gui.hpp
)

# All source files
set(EXE_SOURCE
    ${PROJECT_SOURCE_DIR}/source/visualizer-ui-window.cpp
    ${PROJECT_SOURCE_DIR}/source/gui.cpp
    ${PROJECT_SOURCE_DIR}/source/frame-maker.cpp
    ${PROJECT_SOURCE_DIR}/source/animation-maker.cpp
    ${PROJECT_SOURCE_DIR}/source/async-iteration.cpp
)

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  # Create headers from .ui files with moc
  qt5_wrap_ui(AUTOGEN_UI_SOURCE ${UI_FILES} )
  
  # CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.  
  add_library(${LIB_NAME} STATIC ${EXE_SOURCE} ${AUTOGEN_UI_SOURCE} ${EXE_HPP} ${EXE_QT_HPP})
  
  # Add QT libraries and includes?
  #qt5_use_modules(${EXE_NAME} Core Gui)
  
  set(USED_INCDIRS
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
      ${sotm_INCLUDE_DIRS}
      ${Qt5Widgets_INCLUDE_DIRS}
      ${Qt5Core_INCLUDE_DIRS}
  )
  
  include_directories(${USED_INCDIRS})
  
  target_link_libraries(${LIB_NAME} ${VTK_LIBRARIES} ${sotm_LIBRARY})
  
  export_libs(
      ${LIB_NAME}
  )
  
  export_include_dirs(
      ${PROJECT_SOURCE_DIR}
      ${USED_INCDIRS}
  )
  
else()
    message(FATAL_ERROR "Yor VTK_VERSION < 6 or VTK_QT_VERSION < 4, cannot build")

#  QT4_WRAP_UI(AUTOGEN_UI_SOURCE ${UI_FILES})
#  QT4_WRAP_CPP(MOCSrcs ${EXE_QT_HPP})
#  add_executable(${EXE_NAME} MACOSX_BUNDLE ${EXE_SOURCE} ${AUTOGEN_UI_SOURCE} ${MOCSrcs})
#
#  if(VTK_LIBRARIES)
#    if(${VTK_VERSION} VERSION_LESS "6")
#      target_link_libraries(${EXE_NAME} ${VTK_LIBRARIES} QVTK)
#    else()
#      target_link_libraries(${EXE_NAME} ${VTK_LIBRARIES})
#    endif()
#  else()
#    target_link_libraries(${EXE_NAME} vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
#  endif()
endif()

