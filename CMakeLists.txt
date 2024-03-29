cmake_minimum_required(VERSION 3.4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(detect_version)

find_package(Git)

if (NOT EXISTS external/googletest/CMakeLists.txt)
    if(GIT_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    else()
        message(STATUS "Git not found, please make sure all submodules are updated")
    endif()
endif()

if(APPLE)
    include_directories(SYSTEM /usr/include/malloc)
    if(POLICY CMP0042)
        cmake_policy(SET CMP0042 NEW)
    endif()
endif()

include(static)
include(portable_randomness)

add_library(eacirc-core STATIC
    builtins
    cmd
    dataset
    debug
    iterators
    json
    logger
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
        ${EXTRA_LIBRARIES}
        )

set_target_properties(eacirc-core PROPERTIES
    LINKER_LANGUAGE CXX
    )

detect_version(version.h)
