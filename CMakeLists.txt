cmake_minimum_required(VERSION 3.10)
project(WaraWaraStarter)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source files
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/glad.c
    src/camera.cpp
    src/texture_loader.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    include/GLFW
)

# Link GLFW and OpenGL (Windows uses opengl32)
target_link_directories(${PROJECT_NAME} PRIVATE lib)
target_link_libraries(${PROJECT_NAME}
    glfw3
    opengl32
)