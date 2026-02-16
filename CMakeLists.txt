cmake_minimum_required(VERSION 3.14)
project(physics C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(PHYSICS_BUILD_TESTS "Build test suite" ON)

# Library sources
set(PHYSICS_SOURCES
    src/vec.c
    src/mat.c
    src/kinematics.c
    src/dynamics.c
    src/aerodynamics.c
    src/collision.c
    src/surface.c
)

# Static library
add_library(physics STATIC ${PHYSICS_SOURCES})
target_include_directories(physics PUBLIC include)
target_link_libraries(physics PRIVATE m)

# WASM-specific settings
if(EMSCRIPTEN)
    set(PHYSICS_BUILD_TESTS OFF)
    set_target_properties(physics PROPERTIES
        COMPILE_FLAGS "-O2"
    )
    # Standalone WASM module for JS consumption
    add_executable(physics_wasm src/wasm_bindings.c)
    target_link_libraries(physics_wasm PRIVATE physics)
    set_target_properties(physics_wasm PROPERTIES
        LINK_FLAGS "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] -s MODULARIZE=1 -s EXPORT_NAME='PhysicsModule' -s EXPORT_ES6=1 -s SINGLE_FILE=1 -s ALLOW_MEMORY_GROWTH=1 --no-entry"
        SUFFIX ".js"
    )
endif()

# Tests
if(PHYSICS_BUILD_TESTS AND NOT EMSCRIPTEN)
    enable_testing()
    add_executable(physics_tests
        test/test_main.c
        test/test_vec.c
        test/test_mat.c
        test/test_kinematics.c
        test/test_dynamics.c
        test/test_aerodynamics.c
        test/test_collision.c
        test/test_surface.c
    )
    target_link_libraries(physics_tests PRIVATE physics m)
    add_test(NAME physics_tests COMMAND physics_tests)
endif()

# Examples
option(PHYSICS_BUILD_EXAMPLES "Build examples" OFF)
if(PHYSICS_BUILD_EXAMPLES AND NOT EMSCRIPTEN)
    add_executable(golf_example
        examples/golf/golf_physics.c
        examples/golf/main.c
    )
    target_link_libraries(golf_example PRIVATE physics m)
endif()
