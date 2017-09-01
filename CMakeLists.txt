cmake_minimum_required(VERSION 3.4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(detect_version)

find_package(Git)

if (NOT EXISTS external/googletest/CMakeLists.txt)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

add_library(eacirc-core STATIC
    builtins
    cmd
    dataset
    debug
    iterators
    json
    logger
    memory
    optional
    random
    seed
    statistics
    traits
    variant
    variant
    vec
    view
    )

add_subdirectory(external/json)
add_subdirectory(external/pcg-cpp-0.98)
add_subdirectory(external/googletest)

target_include_directories(eacirc-core INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        )

target_link_libraries(eacirc-core
        json
        pcg
        )

set_target_properties(eacirc-core PROPERTIES
    LINKER_LANGUAGE CXX
    )

detect_version(version.h)
