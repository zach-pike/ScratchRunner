cmake_minimum_required(VERSION 3.30.0)

project(ScratchRunner VERSION 1.0)

add_subdirectory(lib/glad/)
add_subdirectory(lib/stb/)

set(CMAKE_CXX_STANDARD 20)

set(SCRATCH_RUNNER_SRC
    src/GL/Shader/Shader.cpp
    src/GL/ShaderProgram/ShaderProgram.cpp
    src/GL/ShaderLoaders/VertexFragment.cpp

    src/Main.cpp
    src/Scratch/Parse.cpp
    src/ScratchRunner/Runner.cpp
    src/ScratchRunner/Utility/Conv.cpp
    src/ScratchRunner/ThreadedTarget/ThreadedTarget.cpp
    src/ScratchRunner/ThreadedTarget/Worker/Worker.cpp
    src/ScratchRunner/ThreadedTarget/Exec/Exec.cpp
    src/ScratchRunner/ThreadedTarget/Exec/Handlers/Handlers.cpp
)

add_executable(Main ${SCRATCH_RUNNER_SRC})
target_include_directories(Main PRIVATE include/)
target_include_directories(Main PRIVATE ${CMAKE_SOURCE_DIR}/src/)
target_link_directories(Main PRIVATE lib/)
target_link_libraries(Main glfw3 Glad STBImage)
