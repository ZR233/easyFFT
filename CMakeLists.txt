cmake_minimum_required(VERSION 3.21)
project(easyFFT)

set(CMAKE_CXX_STANDARD 11)
option (BUILD_TESTS "Build tests" ON)
option (ENABLE_CL "Use opencl" ON)
if (ENABLE_CL)
    add_compile_definitions(ENABLE_CL)
endif ()

set(BUILD_THIS_TESTS ${BUILD_TESTS})

message(STATUS "BUILD_TESTS=${BUILD_THIS_TESTS}")
message(STATUS "ENABLE_CL=${ENABLE_CL}")
SET(LIBS)
SET(INCLUDE_DIRS)
SET(LIB_DIRS)
SET(PROJECT_STATIC ${PROJECT_NAME}_static)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

add_compile_definitions(CL_TARGET_OPENCL_VERSION=120)
SET(BUILD_EXAMPLES OFF)


IF (UNIX)
    IF (CMAKE_SYSTEM_NAME MATCHES "Android")
        MESSAGE(STATUS "TARGET: Android ")

        if (ENABLE_CL)
            SET(OpenCL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencl)

            if (NOT DEFINED OpenCL_LIBRARY_DIR)
                SET(OpenCL_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_ANDROID_ARCH_ABI})
            endif ()

            if (NOT DEFINED OpenCL_LIBRARY)
                SET(OpenCL_LIBRARY OpenCL)
            endif ()

            message(STATUS OpenCL_LIBRARY_DIR=${OpenCL_LIBRARY_DIR})
            list(APPEND LIB_DIRS ${OpenCL_LIBRARY_DIR})
            list(APPEND LIBS ${OpenCL_LIBRARY})
            list(APPEND INCLUDE_DIRS ${OpenCL_INCLUDE_DIR})
        endif ()

#        if (NOT DEFINED ANDROID_PLATFORM)
#            SET(ANDROID_PLATFORM 26)
#        endif ()
        message(STATUS "ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}")
        message(STATUS "ANDROID_PLATFORM=${ANDROID_PLATFORM}")
    ELSE()
        MESSAGE(STATUS "TARGET: UNIX")
    ENDIF()

    list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fftw-3.3.10/api)

    SET(BUILD_TESTS OFF)
    SET(ANDROID_STL c++_static)
    SET(ENABLE_FLOAT OFF)
    SET(BUILD_SHARED_LIBS OFF)

    add_subdirectory("third_party/fftw-3.3.10/")
    SET(ENABLE_FLOAT ON)
    SET(BUILD_TESTS OFF)
    add_subdirectory("third_party/fftw-3.3.10/" "third_party/fftwf-3.3.10/")

    list(APPEND LIBS fftw3 fftw3f)

    if (ENABLE_CL)
        add_subdirectory("third_party/clfft")
        list(APPEND INCLUDE_DIRS ${CMAKE_BINARY_DIR}/third_party/clfft/include)
        list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/clfft/include)
        list(APPEND LIBS clFFT)
    endif ()

ELSEIF(WIN32)
    MESSAGE(STATUS "TARGET: WIN32")


    if (ENABLE_CL)
        find_package(clFFT CONFIG REQUIRED)
        list(APPEND LIBS clFFT)
    endif ()

ENDIF()


MESSAGE(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

SET(BUILD_EXTERNAL OFF)

find_package(OpenCL)

message(STATUS "OpenCL=${OpenCL_FOUND}")


message(STATUS "CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}")

aux_source_directory(./src SRC_EASY_FFT)
add_library(${PROJECT_STATIC} STATIC ${SRC_EASY_FFT})
add_library(${PROJECT_NAME} SHARED ${SRC_EASY_FFT})

IF(WIN32)
#    add_subdirectory("third_party/fftw-win")
#    add_dependencies(${PROJECT_NAME} fftw3 fftw3f fftw3l)
#    SET(FFT_DLL_DIR third_party/fftw-win)
#    list(APPEND LIB_DIRS ${CMAKE_BINARY_DIR}/third_party/fftw-win)
#    list(APPEND LIBS libfftw3-3 libfftw3f-3 libfftw3l-3)
    add_subdirectory("third_party/fftw-win")
    add_dependencies(${PROJECT_NAME} fftw3 fftw3f)
    add_dependencies(${PROJECT_STATIC} fftw3 fftw3f)
    SET(FFT_DLL_DIR third_party/fftw-win)
    list(APPEND LIB_DIRS ${CMAKE_BINARY_DIR}/third_party/fftw-win)
    list(APPEND LIBS libfftw3-3 libfftw3f-3)

    INSTALL(
            FILES
            ${FFT_DLL_DIR}/libfftw3-3.dll
            ${FFT_DLL_DIR}/libfftw3f-3.dll
            ${FFT_DLL_DIR}/libfftw3l-3.dll DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    SET(FFT_LIB_OUT_DIR ${CMAKE_BINARY_DIR}/third_party/fftw-win)
    INSTALL(
            FILES
            ${FFT_LIB_OUT_DIR}/libfftw3-3.lib
            ${FFT_LIB_OUT_DIR}/libfftw3f-3.lib
            ${FFT_LIB_OUT_DIR}/libfftw3l-3.lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
ENDIF()

message(STATUS "INCLUDE_DIRS=${INCLUDE_DIRS}")
message(STATUS "LIB_DIRS=${LIB_DIRS}")
target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
message(STATUS "LIBS: ${LIBS}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

#target_link_directories(${PROJECT_STATIC} PUBLIC ${LIB_DIRS})
link_directories( ${LIB_DIRS})
target_include_directories(${PROJECT_STATIC} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_STATIC} PRIVATE ${LIBS})
#link_libraries(${PROJECT_STATIC} PUBLIC ${LIBS})

INSTALL(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/easyFFT.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS ${PROJECT_STATIC}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(BUILD_THIS_TESTS)
#if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_subdirectory(test)
endif()



