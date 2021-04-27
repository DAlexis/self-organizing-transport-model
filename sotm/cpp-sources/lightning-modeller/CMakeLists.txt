cmake_minimum_required(VERSION 2.8)

project(lightmod)

find_package (Boost COMPONENTS date_time program_options REQUIRED)

set(EXE_SOURCES
    main.cpp
    modeller.cpp
    seeds-generator.cpp
    coulomb-selector.cpp
    
    modeller.hpp
    seeds-generator.hpp
    coulomb-selector.hpp
)


add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PUBLIC cic)

target_link_libraries (${PROJECT_NAME} PUBLIC
    sotm
    sotm-gui
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

add_custom_target(
    gen-config
    COMMAND ${PROJECT_SOURCE_DIR}/gen-config.sh ${PROJECT_SOURCE_DIR}
)

add_dependencies(${PROJECT_NAME} gen-config)

configure_file(plot-field.sh plot-field.sh COPYONLY)
configure_file(plot-field-all.sh plot-field-all.sh COPYONLY)
