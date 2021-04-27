cmake_minimum_required(VERSION 2.8)

project(lightning-modeller-unit-tests)

find_package (Boost COMPONENTS program_options REQUIRED)

include_directories (
    ${GTEST_INCLUDE_DIR}
    ${sotm_INCLUDE_DIRS}
    ${lightmod_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
    coulomb-selector-ut.cpp
    ../../lightning-modeller/coulomb-selector.cpp
    ../../lightning-modeller/coulomb-selector.hpp
)

target_link_libraries (${PROJECT_NAME} PUBLIC
    gtest
    gtest_main
    sotm
    sotm-gui
    cic
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)


add_test(NAME lightmod_testing COMMAND ${PROJECT_NAME})
