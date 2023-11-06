cmake_minimum_required(VERSION 3.22.1)


set(CXX_STANDARD 20)
project(ScratchRunner)

include_directories(src/)

# Hate this hack but it works
include_directories(external/)
include_directories(external/imgui)

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB IMGUI_SRC external/imgui/*.cpp)
set(IMGUI_IMPL_SRC external/imgui/backends/imgui_impl_glfw.cpp external/imgui/backends/imgui_impl_opengl3.cpp)

# Append source for imgui to code
add_executable(main ${SRC} ${IMGUI_SRC} ${IMGUI_IMPL_SRC})

target_link_libraries(main GL GLEW glfw m X11 Xi Xrandr)