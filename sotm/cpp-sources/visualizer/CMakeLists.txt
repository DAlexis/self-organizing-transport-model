
cmake_minimum_required(VERSION 2.8)
 
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

PROJECT(sotm-visualizer)

set(EXE_NAME ${PROJECT_NAME})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
  find_package(Qt5Widgets REQUIRED QUIET)
else()
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# All .ui files
file(GLOB UI_FILES *.ui)
#file(GLOB QT_WRAP *.hpp)
#file(GLOB CXX_FILES *.cpp)

# Headers wirth Q_OBJECTs
set(EXE_QT_HPP visualizer-ui-window.hpp)

# Headers without Q_OBJECT  
set(EXE_HPP graph-wireframe-drawer.hpp)

# All source files
set(EXE_SOURCE
    visualizer-ui-window.cpp
    visualizer-ui-window-driver.cpp
    graph-wireframe-drawer.cpp
)

include_directories(${sotm_INCLUDE_DIRS})

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  
  # Create headers from .ui files with moc
  qt5_wrap_ui(AUTOGEN_UI_SOURCE ${UI_FILES} )
  
  # CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
  add_executable(${EXE_NAME} MACOSX_BUNDLE ${EXE_SOURCE} ${AUTOGEN_UI_SOURCE} ${EXE_HPP} ${EXE_QT_HPP})
  
  # Add QT libraries and includes?
  qt5_use_modules(${EXE_NAME} Core Gui)
  
  target_link_libraries(${EXE_NAME} ${VTK_LIBRARIES} ${sotm_LIBRARY})
  
else()
  QT4_WRAP_UI(AUTOGEN_UI_SOURCE ${UI_FILES})
  QT4_WRAP_CPP(MOCSrcs ${EXE_QT_HPP})
  add_executable(${EXE_NAME} MACOSX_BUNDLE ${EXE_SOURCE} ${AUTOGEN_UI_SOURCE} ${MOCSrcs})

  if(VTK_LIBRARIES)
    if(${VTK_VERSION} VERSION_LESS "6")
      target_link_libraries(${EXE_NAME} ${VTK_LIBRARIES} QVTK)
    else()
      target_link_libraries(${EXE_NAME} ${VTK_LIBRARIES})
    endif()
  else()
    target_link_libraries(${EXE_NAME} vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
  endif()
endif()

