cmake_minimum_required(VERSION 3.21)
project(easyFFT)

set(CMAKE_CXX_STANDARD 11)
option (BUILD_TESTS "Build tests" ON)
SET(LIBS)
SET(INCLUDE_DIRS)
SET(LIB_DIRS)
SET(PROJECT_STATIC ${PROJECT_NAME}_static)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vkFFT)

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fftw-3.3.10/api)
IF (CMAKE_SYSTEM_NAME MATCHES "Android")
    MESSAGE(STATUS "current platform: Android ")
    if (NOT DEFINED ANDROID_PLATFORM)
        SET(ANDROID_PLATFORM 26)
    endif ()
    set(BUILD_TESTS OFF)
    SET(ANDROID_STL c++_static)
    set(ENABLE_FLOAT OFF)
    set(BUILD_SHARED_LIBS OFF)

    message(STATUS "ANDROID_PLATFORM=${ANDROID_PLATFORM}")
    add_subdirectory("third_party/fftw-3.3.10/")
    set(ENABLE_FLOAT ON)
    set(BUILD_TESTS OFF)
    add_subdirectory("third_party/fftw-3.3.10/" "third_party/fftwf-3.3.10/")

    list(APPEND LIBS fftw3 fftw3f)
endif()
IF(MSVC)
    MESSAGE(STATUS "current platform: MSVC")
ENDIF()

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

SET(BUILD_EXTERNAL OFF)
find_package(OpenCL)
find_package(Vulkan)


message(STATUS "OpenCL=${OpenCL_FOUND}")
message(STATUS "Vulkan=${Vulkan_FOUND}")


if (OpenCL_FOUND)
    SET(VKFFT_BACKEND_BK 3)
endif()
if (Vulkan_FOUND)
    SET(VKFFT_BACKEND_BK 0)
endif()

if (NOT DEFINED VKFFT_BACKEND)
    SET(VKFFT_BACKEND ${VKFFT_BACKEND_BK})
endif ()

message(STATUS "VKFFT_BACKEND=${VKFFT_BACKEND}")
add_definitions(-DVKFFT_BACKEND=${VKFFT_BACKEND})

message(STATUS "CMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}")

aux_source_directory(./src SRC_EASY_FFT)
add_library(${PROJECT_STATIC} STATIC ${SRC_EASY_FFT})
add_library(${PROJECT_NAME} SHARED ${SRC_EASY_FFT})


if (${VKFFT_BACKEND} EQUAL 0)
    if(NOT ${Vulkan_FOUND})
        message(FATAL_ERROR "Vulkan not Found")
    endif()
    set(BUILD_EXTERNAL OFF)

    list(APPEND LIBS Vulkan::Vulkan SPIRV glslang)
    list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glslang-master-tot/glslang/Include/)
    #    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glslang-master-tot/glslang/Include/)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glslang-master-tot)
endif ()

if (${VKFFT_BACKEND} EQUAL 3)
    if(NOT ${OpenCL_FOUND})
        message(FATAL_ERROR "OpenCL not Found")
    endif()
    list(APPEND INCLUDE_DIRS ${OpenCL_INCLUDE_DIRS})
    #    target_include_directories(${PROJECT_NAME} PUBLIC OpenCL_INCLUDE_DIRS)
    list(APPEND LIBS OpenCL::OpenCL)
endif ()

IF(MSVC)
    add_subdirectory("third_party/fftw-win")
    add_dependencies(${PROJECT_NAME} fftw3 fftw3f fftw3l)
    SET(FFT_DLL_DIR third_party/fftw-win)
    list(APPEND LIB_DIRS ${CMAKE_BINARY_DIR}/third_party/fftw-win)
    list(APPEND LIBS libfftw3-3 libfftw3f-3 libfftw3l-3)


    INSTALL(
            FILES
            ${FFT_DLL_DIR}/libfftw3-3.dll
            ${FFT_DLL_DIR}/libfftw3f-3.dll
            ${FFT_DLL_DIR}/libfftw3l-3.dll DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
ENDIF()
message(STATUS "INCLUDE_DIRS=${INCLUDE_DIRS}")
message(STATUS "LIB_DIRS=${LIB_DIRS}")
target_link_directories(${PROJECT_NAME} PUBLIC ${LIB_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
message(STATUS "LIBS: ${LIBS}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

target_link_directories(${PROJECT_STATIC} PUBLIC ${LIB_DIRS})
target_include_directories(${PROJECT_STATIC} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_STATIC} PRIVATE ${LIBS})

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


if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_subdirectory(test)
endif()



