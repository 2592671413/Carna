if( ${CMAKE_COMPILER_IS_GNUCC} )
    set( GOMP_LIBRARIES "gomp" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wno-enum-compare" )
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if( GCC_VERSION VERSION_LESS 4.7 )
        add_definitions( -DNO_OVERRIDE_KEYWORD )
    endif()
    if( GCC_VERSION VERSION_GREATER 4.9 OR GCC_VERSION VERSION_EQUAL 4.9 )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always" )
    endif()
    if(Qt5_FOUND)
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
    endif()
endif()
if( WIN32 )
    set( OPENGL_LIBRARIES opengl32 glu32 )
else()
    set( OPENGL_LIBRARIES GL GLU )
endif()

