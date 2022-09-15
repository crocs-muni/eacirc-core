if (NOT DEFINED STATIC)
    set(STATIC ON)
endif()

if(STATIC)
    if(MSVC)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll.a .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    endif()
endif()

if(NOT MSVC)
    if(STATIC)
        if (MINGW)
            # On Windows, this is as close to fully-static as we get:
            # this leaves only deps on /c/Windows/system32/*.dll
            set(STATIC_FLAGS "-static")
        elseif (NOT (APPLE OR FREEBSD OR OPENBSD OR DRAGONFLY))
            set(STATIC_FLAGS "-static-libgcc -static-libstdc++")
        endif()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${STATIC_FLAGS} ")
    endif()
endif()
