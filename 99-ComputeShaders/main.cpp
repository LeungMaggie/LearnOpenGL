#include "ComputeShader.h"
#include <GLFW/glfw3.h>
#include <iostream>

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

int main(int argc, char** argv)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "ComputeShader", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(0); // 0：关闭垂直同步
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  //! query limitations
  int max_compute_work_group_count[3];
  int max_compute_work_group_size[3];
  int max_compute_work_group_invocations;
  std::string xyz = "XYZ";
  std::cout << "OpenGL Limitations: " << std::endl;
  for (int idx = 0; idx < 3; ++idx)
  {
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
    std::cout << "maximum number of work groups in " << xyz[idx] << " dimension " << max_compute_work_group_count[idx] << std::endl;
    std::cout << "maximum size of a work group in " << xyz[idx] << " dimension " << max_compute_work_group_size[idx] << std::endl;
  }
  glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);
  std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;

  Shader shader("../screenQuad.vs", "../screenQuad.fs");
  ComputeShader compShader("../computeShader.comp");

  shader.use();
  shader.set_int("tex", 0);

  const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;
  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  // bind a specific level of a texture to an image unit
  glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  float quadVertices[] = {
    // positions        // texture coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
  };

  unsigned int VBO, VAO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  int fCounter = 0;
  float lastFrame = 0.0f; // time of last frame
  float deltaTime = 0.0f; // time between current frame and last frame
  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    if (fCounter > 500)
    {
      std::cout << "FPS: " << 1 / deltaTime << std::endl;
      fCounter = 0;
    }
    else
    {
      fCounter++;
    }

    compShader.use();
    compShader.set_float("t", currentFrame);
    // dispatch number of work groups
    // go to .comp to adjust the local_size of the compute shader accordingly
    // so that the image expands to full screen
    glDispatchCompute((unsigned int)TEXTURE_WIDTH/10, (unsigned int)TEXTURE_HEIGHT/10, 1);
    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    // render image to quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteTextures(1, &texture);
  glDeleteProgram(shader.ID);
  glDeleteProgram(compShader.ID);

  glfwTerminate();
  return 0;
}

