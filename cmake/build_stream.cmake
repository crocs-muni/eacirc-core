function(build_stream TARGET NAME)
    string(TOUPPER ${NAME} UPPERCASE_NAME)

    option(BUILD_${NAME} "Build stream ${NAME} for ${TARGET}" ON)

    if(BUILD_${NAME})
        target_link_libraries(${TARGET} ${NAME})
        add_definitions(-DBUILD_${NAME})
    endif()
endfunction()
