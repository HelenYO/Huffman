cmake_minimum_required(VERSION 3.0)
project(Huffman)

include_directories(${huffman_SOURCE_DIR})

add_executable(huffman-util main.cpp)
add_executable(huffman-testing huffman_testing.cpp
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc)


add_library(huffman SHARED lib/huffman.h lib/huffman.cpp lib/istream_cover.cpp lib/istream_cover.h lib/ostream_cover.cpp lib/ostream_cover.h)
include_directories(lib)



target_link_libraries(huffman-util huffman)
target_link_libraries(huffman-testing huffman -lpthread)