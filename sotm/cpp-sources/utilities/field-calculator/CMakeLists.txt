cmake_minimum_required(VERSION 2.8)

project(field-calc)

find_package (Boost COMPONENTS date_time program_options REQUIRED)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(EXE_SOURCES
    main.cpp
    field-calculator.hpp
    field-calculator.cpp
    
    grid-builder.cpp
    grid-builder.hpp
)

include_directories(
    ${sotm_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    tbb
    sotm
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

if(VTK_LIBRARIES)
  target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME} vtkHybrid vtkWidgets)
endif()
