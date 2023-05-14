project(sotm-unit-tests)

set(EXE_SOURCES
    math/electromagnetic-emission-ut.cpp
    math/generic-ut.cpp
    math/geometry-ut.cpp
    math/integration-ut.cpp
    math/distrib-gen-ut.cpp
    math/field-ut.cpp
    math/functions-ut.cpp
    base/transport-graph-ut.cpp
    output/variables-ut.cpp
    utils/memory-ut.cpp
    payloads/demo/empty-payload-ut.cpp
    time-iter/euler-explicit-ut.cpp
    time-iter/runge-kutta-ut.cpp
    time-iter/exponent-time-iterable.cpp
    complex/branching-ut.cpp
)

set(EXE_HPP
    complex/branching-trivial-physics.hpp
    time-iter/exponent-time-iterable.hpp
    complex/branching-trivial-physics.hpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries (${PROJECT_NAME} PUBLIC
    gtest
    gtest_main
    sotm
)

#target_include_directories (${PROJECT_NAME} PUBLIC
#    ${GTEST_INCLUDE_DIR}
#    ${sotm_INCLUDE_DIRS}
#)


add_test(NAME Library_testing COMMAND ${PROJECT_NAME})
configure_file(math/plot-emission.py plot-emission.py COPYONLY)
