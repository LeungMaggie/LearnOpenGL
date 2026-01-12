cmake_minimum_required(VERSION 3.10)
project(OpenGLHelloWindow)

set(GL_LIB_PATH "C:/Maggie/CG/LearnOpenGL/libraries")
include_directories("${GL_LIB_PATH}/include")
link_directories("${GL_LIB_PATH}/lib")

set(EXE_NAME "HelloWindow")
add_executable(${EXE_NAME} main.cpp src/glad.c)
target_link_libraries(${EXE_NAME} glfw3 opengl32.lib)

