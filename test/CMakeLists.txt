cmake_minimum_required(VERSION 3.20)
project(Google_tests)


set(CMAKE_CXX_STANDARD 11)

# 'lib' is the folder with Google Test sources
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ../src/include)

aux_source_directory(. SRC_TEST)
add_executable(Google_Tests_run  ${SRC_TEST})


target_link_libraries(Google_Tests_run  gtest gtest_main easyFFT)





