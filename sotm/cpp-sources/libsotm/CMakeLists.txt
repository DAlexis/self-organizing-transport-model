cmake_minimum_required(VERSION 2.8)

project(sotm)

find_package(VTK
    REQUIRED
)


set(LIB_SOURCE
    source/math/generic.cpp
    source/math/geometry.cpp
    source/math/random.cpp
    source/math/integration.cpp
    source/math/distrib-gen.cpp
    source/math/functions.cpp
    sotm/math/electric-emission.hpp
    source/base/transport-graph.cpp
    source/base/physical-payload.cpp
    source/base/model-context.cpp
    source/base/parallel.cpp
    source/base/time-iter.cpp
    source/base/parameters.cpp
    source/output/graph-renderer.cpp
    source/output/graph-file-writer.cpp
    source/output/variables.cpp
    source/time-iter/euler-explicit.cpp
    source/time-iter/runge-kutta.cpp
    source/payloads/demo/empty-payloads.cpp
    source/payloads/demo/absolute-random-graph.cpp
    source/payloads/electrostatics/electrostatics.cpp
    source/payloads/electrostatics/electrostatics-scaler.cpp
    source/optimizers/coulomb.cpp
    source/optimizers/coulomb-brute-force.cpp
    source/optimizers/coulomb-octree.cpp
)

set(LIB_HPP
    sotm/payloads/electrostatics/electrostatics.hpp
    sotm/payloads/electrostatics/electrostatics-scaler.hpp
    sotm/payloads/demo/empty-payloads.hpp
    sotm/payloads/demo/absolute-random-graph.hpp
    sotm/time-iter/runge-kutta.hpp
    sotm/time-iter/euler-explicit.hpp
    sotm/math/random.hpp
    sotm/math/integration.hpp
    sotm/math/generic.hpp
    sotm/math/geometry.hpp
    sotm/math/distrib-gen.hpp
    sotm/math/functions.hpp
    source/math/electric-emission.cpp
    sotm/output/graph-renderer.hpp
    sotm/output/graph-file-writer.hpp
    sotm/utils/memory.hpp
    sotm/utils/assert.hpp
    sotm/utils/macros.hpp
    sotm/utils/utils.hpp
    sotm/utils/const.hpp
    sotm/base/model-context.hpp
    sotm/base/time-iter.hpp
    sotm/base/physical-payload.hpp
    sotm/base/transport-graph.hpp
    sotm/base/parameters.hpp
    sotm/optimizers/coulomb.hpp
    sotm/optimizers/coulomb-brute-force.hpp
    sotm/optimizers/coulomb-octree.hpp
)


add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HPP})

include(${VTK_USE_FILE})

target_link_libraries(${PROJECT_NAME} PUBLIC ${VTK_LIBRARIES} tbb octree)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
