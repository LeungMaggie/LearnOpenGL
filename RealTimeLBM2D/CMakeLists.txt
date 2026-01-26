cmake_minimum_required(VERSION 3.10)
project(OpenGLRealTimeLBM2D)

set(CMAKE_CXX_STANDARD 17)

set(GL_LIB_PATH "C:/Maggie/CG/LearnOpenGL/libraries")
include_directories("${GL_LIB_PATH}/include")
link_directories("${GL_LIB_PATH}/lib")

set(EXE_NAME "RealTimeLBM2D")
add_executable(
    ${EXE_NAME}
    main.cpp
    ComputeShader.h
    ComputeShader.cpp
    Shader.h
    Shader.cpp
    GLHelpers.h
    GLHelpers.cpp
    ${GL_LIB_PATH}/src/glad.c
    ${GL_LIB_PATH}/src/stb_image.cpp
)
target_link_libraries(${EXE_NAME} glfw3 opengl32.lib)
