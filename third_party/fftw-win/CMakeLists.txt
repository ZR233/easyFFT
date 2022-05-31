cmake_minimum_required(VERSION 3.23)
project(fftw-win)

IF(${CMAKE_CL_64})
    SET(FFTW_LIB_CMD /machine:X64)
else()
    SET(FFTW_LIB_CMD " ")
endif()

add_custom_command(
        OUTPUT libfftw3-3.lib
        COMMAND lib.exe ${FFTW_LIB_CMD} /def:${CMAKE_CURRENT_SOURCE_DIR}/libfftw3-3.def
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libfftw3-3.def
        VERBATIM)

add_custom_target(fftw3 ALL
        DEPENDS libfftw3-3.lib
        VERBATIM)

add_custom_command(
        OUTPUT libfftw3f-3.lib
        COMMAND lib.exe ${FFTW_LIB_CMD} /def:${CMAKE_CURRENT_SOURCE_DIR}/libfftw3f-3.def
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libfftw3f-3.def
        VERBATIM)

add_custom_target(fftw3f ALL
        DEPENDS libfftw3f-3.lib
        VERBATIM)
add_custom_command(
        OUTPUT libfftw3l-3.lib
        COMMAND lib.exe ${FFTW_LIB_CMD} /def:${CMAKE_CURRENT_SOURCE_DIR}/libfftw3l-3.def
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libfftw3l-3.def
        VERBATIM)

add_custom_target(fftw3l ALL
        DEPENDS libfftw3l-3.lib
        VERBATIM)