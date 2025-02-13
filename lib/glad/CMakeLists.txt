cmake_minimum_required(VERSION 3.30.0)

add_library(Glad STATIC glad.c)
target_include_directories(Glad PUBLIC ${CMAKE_SOURCE_DIR}/include/)