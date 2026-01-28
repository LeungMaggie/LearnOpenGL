#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void process_input(GLFWwindow* window);

GLFWwindow* gl_init_window(
  const unsigned int& width,
  const unsigned int& height,
  const std::string& title);

void gl_init_obstable_tex_fbo(
  unsigned int& obstacleTexID,
  unsigned int& obstacleFboID,
  const unsigned int& width,
  const unsigned int& height);

void gl_init_texture(
  unsigned int& texID,
  const unsigned int& width,
  const unsigned int& height,
  const GLenum& glTexture);

void gl_init_vao_vbo(
  unsigned int& vaoID,
  unsigned int& vboID,
  const std::vector<int>& attribComps,
  const GLsizeiptr& bufferSize,
  const void* bufferDataPtr);
