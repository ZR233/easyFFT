cmake_minimum_required(VERSION 3.23)
project(rfftc)

set(CMAKE_CXX_STANDARD 11)
SET(LIBS)
SET(INCLUDE_DIRS)
SET(LIB_DIRS)
#SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_SYSTEM_NAME})


IF (CMAKE_SYSTEM_NAME MATCHES "Android")
    MESSAGE(STATUS "current platform: Android ")
    if (NOT DEFINED ANDROID_PLATFORM)
        SET(ANDROID_PLATFORM 26)
    endif ()
    SET(ANDROID_STL c++_static)
    message(STATUS "ANDROID_PLATFORM=${ANDROID_PLATFORM}")
    add_subdirectory("third_party/fftw-3.3.10")
#    SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${ANDROID_NDK_ABI_NAME})
    message(STATUS "CMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}")
    list(APPEND LIBS ${FFTW3_LIBRARIES})
endif()
IF(MSVC)
    MESSAGE(STATUS "current platform: MSVC")
    SET(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${CMAKE_C_COMPILER_ARCHITECTURE_ID})
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

add_library(${PROJECT_NAME} SHARED src/include/rfft.h src/rfft.cpp src/PlanFloat.cpp src/PlanFloat.h)
IF (CMAKE_SYSTEM_NAME MATCHES "Android")
    list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fftw-win)
    add_dependencies(${PROJECT_NAME} fftw3)
endif()


if (${VKFFT_BACKEND} EQUAL 0)
    if(NOT ${Vulkan_FOUND})
        message(FATAL_ERROR "Vulkan not Found")
    endif()
    set(BUILD_EXTERNAL OFF)
    cmake_policy(SET CMP0077 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
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
    find_package(FFTW3 CONFIG REQUIRED)
    find_package(FFTW3f CONFIG REQUIRED)
    find_package(FFTW3l CONFIG REQUIRED)


    list(APPEND LIBS FFTW3::fftw3 FFTW3::fftw3f FFTW3::fftw3l)

#    list(APPEND LIB_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fftw-win)
#    add_dependencies(${PROJECT_NAME} fftw3 fftw3f fftw3l)

    INSTALL(
            FILES ${CMAKE_BINARY_DIR}/rfftc.dll DESTINATION C:\\Windows\\system32
    )
ENDIF()
message(STATUS "INCLUDE_DIRS=${INCLUDE_DIRS}")
target_link_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
message(STATUS "LIBS: ${LIBS}")
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBS})

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})


