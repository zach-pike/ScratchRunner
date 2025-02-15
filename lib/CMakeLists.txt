cmake_minimum_required(VERSION 3.30.0)

add_subdirectory(stb/)
add_subdirectory(glad/)

set(IMGUI_FILES
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_library(ImGui STATIC ${IMGUI_FILES})
target_include_directories(ImGui PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(ImGui PUBLIC imgui/)