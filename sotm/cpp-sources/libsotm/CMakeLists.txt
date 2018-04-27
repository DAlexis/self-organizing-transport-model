cmake_minimum_required(VERSION 2.8)

project(sotm)

find_package(VTK REQUIRED)
#include(${VTK_USE_FILE})


set(LIB_SOURCE
    ${PROJECT_SOURCE_DIR}/source/math/generic.cpp
    ${PROJECT_SOURCE_DIR}/source/math/geometry.cpp
    ${PROJECT_SOURCE_DIR}/source/math/random.cpp
    ${PROJECT_SOURCE_DIR}/source/math/integration.cpp
    ${PROJECT_SOURCE_DIR}/source/math/distrib-gen.cpp
    ${PROJECT_SOURCE_DIR}/source/math/functions.cpp
    ${PROJECT_SOURCE_DIR}/source/base/transport-graph.cpp
    ${PROJECT_SOURCE_DIR}/source/base/physical-payload.cpp
    ${PROJECT_SOURCE_DIR}/source/base/model-context.cpp
    ${PROJECT_SOURCE_DIR}/source/base/parallel.cpp
    ${PROJECT_SOURCE_DIR}/source/base/time-iter.cpp
    ${PROJECT_SOURCE_DIR}/source/base/parameters.cpp
    ${PROJECT_SOURCE_DIR}/source/output/graph-renderer.cpp
    ${PROJECT_SOURCE_DIR}/source/output/graph-file-writer.cpp
    ${PROJECT_SOURCE_DIR}/source/output/variables.cpp
    ${PROJECT_SOURCE_DIR}/source/time-iter/euler-explicit.cpp
    ${PROJECT_SOURCE_DIR}/source/time-iter/runge-kutta.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/demo/empty-payloads.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/demo/absolute-random-graph.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/electrostatics/electrostatics.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/electrostatics/electrostatics-scaler.cpp
	${PROJECT_SOURCE_DIR}/source/optimizers/coulomb.cpp
)

set(LIB_HPP
    ${PROJECT_SOURCE_DIR}/sotm/payloads/electrostatics/electrostatics.hpp
    ${PROJECT_SOURCE_DIR}/sotm/payloads/electrostatics/electrostatics-scaler.hpp
    ${PROJECT_SOURCE_DIR}/sotm/payloads/demo/empty-payloads.hpp
    ${PROJECT_SOURCE_DIR}/sotm/payloads/demo/absolute-random-graph.hpp
    ${PROJECT_SOURCE_DIR}/sotm/time-iter/runge-kutta.hpp
    ${PROJECT_SOURCE_DIR}/sotm/time-iter/euler-explicit.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/random.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/integration.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/generic.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/geometry.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/distrib-gen.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/functions.hpp
    ${PROJECT_SOURCE_DIR}/sotm/output/graph-renderer.hpp
    ${PROJECT_SOURCE_DIR}/sotm/output/graph-file-writer.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/memory.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/assert.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/macros.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/utils.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/const.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/model-context.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/time-iter.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/physical-payload.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/transport-graph.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/parameters.hpp
	${PROJECT_SOURCE_DIR}/sotm/optimizers/coulomb.hpp
	${PROJECT_SOURCE_DIR}/sotm/optimizers/coulomb-calculator.hpp
)

# Include dirs that are used and will be exported 
set(${PROJECT_NAME}_USED_INCDIRS
    ${PROJECT_SOURCE_DIR}
	${octree_INCLUDE_DIRS}
)

include_directories(${${PROJECT_NAME}_USED_INCDIRS})

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE} ${LIB_HPP})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

# Include directories that should be added with this project
export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${${PROJECT_NAME}_USED_INCDIRS}
)

# Libraries this projects needs to be linked with
export_libs(
    ${PROJECT_NAME}
	${octree_LIBRARY}
	tbb
)
