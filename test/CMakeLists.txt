project(Google_tests)


set(CMAKE_CXX_STANDARD 11)

enable_testing()
if (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif ()

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ../src/include)

aux_source_directory(. SRC_TEST)
add_executable(Google_Tests_run  ${SRC_TEST})
target_link_libraries(Google_Tests_run gtest_main easyFFT_static)





