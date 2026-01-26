#include "GLHelpers.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

GLFWwindow* gl_init_window(
  const unsigned int& width,
  const unsigned int& height,
  const std::string& title)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // glfwSetCursorPosCallback(window, mouse_callback);
  // glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    exit(EXIT_FAILURE);
  }

  return window;
}

void gl_init_texture(
  unsigned int& texID,
  const unsigned int& width,
  const unsigned int& height,
  const GLenum& glTexture)
{
  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(glTexture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glActiveTexture(glTexture);
  glBindTexture(GL_TEXTURE_2D, texture);
  texID = texture;
}

void gl_init_vao_vbo(
  unsigned int& vaoID,
  unsigned int& vboID,
  const std::vector<int>& attribComps,
  const GLsizeiptr& bufferSize,
  const void* bufferDataPtr)
{
  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferDataPtr, GL_STATIC_DRAW);
  int stride = 0;
  for (int i = 0; i < attribComps.size(); ++i)
  {
    stride += attribComps[i];
  }

  int ptrPos = 0;
  for (int i = 0; i < attribComps.size(); ++i)
  {
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, attribComps[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(ptrPos * sizeof(float)));
    ptrPos += attribComps[i];
  }
  vaoID = vao;
  vboID = vbo;
}
