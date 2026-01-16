cmake_minimum_required(VERSION 3.10)
project(OpenGLShaders)

set(GL_LIB_PATH "C:/Maggie/CG/LearnOpenGL/libraries")
include_directories("${GL_LIB_PATH}/include")
link_directories("${GL_LIB_PATH}/lib")

set(EXE_NAME "Shaders")
# add_executable(${EXE_NAME} Shaders.cpp src/glad.c)
add_executable(${EXE_NAME} main.cpp Shader.h Shader.cpp src/glad.c)
target_link_libraries(${EXE_NAME} glfw3 opengl32.lib)
