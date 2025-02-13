cmake_minimum_required(VERSION 3.30.0)

add_library(STBImage STATIC stb_impl.c)
target_include_directories(STBImage PUBLIC ${CMAKE_SOURCE_DIR}/include)