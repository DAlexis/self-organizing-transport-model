cmake_minimum_required(VERSION 2.8)

project(sotm)

set(LIB_SOURCE
    ${PROJECT_SOURCE_DIR}/source/math/generic.cpp
    ${PROJECT_SOURCE_DIR}/source/math/geometry.cpp
    ${PROJECT_SOURCE_DIR}/source/math/random.cpp
    ${PROJECT_SOURCE_DIR}/source/math/integration.cpp
    ${PROJECT_SOURCE_DIR}/source/math/distrib-gen.cpp
    ${PROJECT_SOURCE_DIR}/source/base/transport-graph.cpp
    ${PROJECT_SOURCE_DIR}/source/base/physical-payload.cpp
    ${PROJECT_SOURCE_DIR}/source/base/model-context.cpp
    ${PROJECT_SOURCE_DIR}/source/base/time-iter.cpp
    ${PROJECT_SOURCE_DIR}/source/time-iter/euler-explicit.cpp
    ${PROJECT_SOURCE_DIR}/source/time-iter/runge-kutta.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/demo/empty-payloads.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/demo/absolute-random-graph.cpp
    ${PROJECT_SOURCE_DIR}/source/payloads/electrostatics/electrostatics-simple.cpp
)

set(LIB_HPP
    ${PROJECT_SOURCE_DIR}/sotm/payloads/demo/empty-payloads.hpp
    ${PROJECT_SOURCE_DIR}/sotm/payloads/demo/absolute-random-graph.hpp
    ${PROJECT_SOURCE_DIR}/sotm/time-iter/euler-explicit.hpp
    ${PROJECT_SOURCE_DIR}/sotm/time-iter/runge-kutta.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/geometry.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/random.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/generic.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/integration.hpp
    ${PROJECT_SOURCE_DIR}/sotm/math/distrib-gen.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/memory.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/assert.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/macros.hpp
    ${PROJECT_SOURCE_DIR}/sotm/utils/utils.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/model-context.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/time-iter.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/physical-payload.hpp
    ${PROJECT_SOURCE_DIR}/sotm/base/transport-graph.hpp
)

# Include dirs that are used and will be exported 
set(${PROJECT_NAME}_USED_INCDIRS
    ${PROJECT_SOURCE_DIR}
)

include_directories(${${PROJECT_NAME}_USED_INCDIRS})

add_library(${PROJECT_NAME} STATIC ${LIB_SOURCE} ${LIB_HPP})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_rvalue_references)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_lambdas)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
    ${${PROJECT_NAME}_USED_INCDIRS}
    # Add here include directories that should be exported, i.e. ${PROJECT_SOURCE_DIR}/include
)

export_libs(
    ${PROJECT_NAME}
    # Add here libraries this one depends from
)
