cmake_minimum_required(VERSION 2.8)

project(lightmod)

find_package (Boost COMPONENTS date_time program_options REQUIRED)

set(EXE_SOURCES
    main.cpp
    modeller.hpp
    modeller.cpp
)

include_directories(
    ${sotm_INCLUDE_DIRS}
    ${sotm-gui_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    ${sotm_LIBRARY}
    ${sotm-gui_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY} # For command line options parsing for example
)

