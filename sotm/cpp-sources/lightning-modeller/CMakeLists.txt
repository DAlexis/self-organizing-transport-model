cmake_minimum_required(VERSION 2.8)

project(lightmod)

find_package (Boost COMPONENTS date_time program_options REQUIRED)

set(EXE_SOURCES
    main.cpp
    modeller.cpp
    
    modeller.hpp
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

add_custom_target(
    gen-config
    COMMAND ${PROJECT_SOURCE_DIR}/gen-config.sh ${PROJECT_SOURCE_DIR}
)

add_dependencies(${PROJECT_NAME} gen-config)

configure_file(plot-field.sh plot-field.sh COPYONLY)
configure_file(plot-field-all.sh plot-field-all.sh COPYONLY)
