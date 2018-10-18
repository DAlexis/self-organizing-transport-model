cmake_minimum_required(VERSION 2.8)

project(lightning-modeller-unit-tests)

find_package (Boost COMPONENTS program_options REQUIRED)

include_directories (
    ${GTEST_INCLUDE_DIR}
    ${sotm_INCLUDE_DIRS}
    ${lightmod_INCLUDE_DIRS}
)

set(EXE_SOURCES
    coulomb-selector-ut.cpp
    ../../lightning-modeller/coulomb-selector.cpp
    ../../lightning-modeller/coulomb-selector.hpp
)

set(EXE_HPP
    
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
    ${sotm_LIBRARY}
	${sotm-gui_LIBRARY}
	${cic_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)


add_test(NAME lightmod_testing COMMAND ${PROJECT_NAME})
