cmake_minimum_required(VERSION 2.8)

project(lightning-model)

find_package (Boost COMPONENTS date_time program_options REQUIRED)

set(EXE_SOURCES
    main.cpp
)

include_directories(
    {sotm_INCLUDE_DIRS}
    ${sotm-gui_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    ${sotm_LIBRARY}
    ${sotm-gui_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

configure_file(parameters.ini parameters.ini COPYONLY)


