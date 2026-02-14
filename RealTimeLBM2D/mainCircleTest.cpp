#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLHelpers.h"
#include "Shader.h"
#include "ComputeShader.h"

int main()
{
  unsigned int windowWidth = 800;
  unsigned int windowHeight = 800;
  std::string windowTitle = "RealTimeLBM2D";
  auto window = gl_init_window(windowWidth, windowHeight, windowTitle);

  unsigned int texOutput;
  unsigned int texWidth = 8;
  unsigned int texHeight = 8;
  gl_init_texture(texOutput, texWidth + 2, texHeight + 2, GL_TEXTURE0);

  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texOutput, 0);

  unsigned int ssboDebug;
  glGenBuffers(1, &ssboDebug);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDebug);
  glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  ComputeShader compShader("../circle.comp");
  compShader.use();
  compShader.set_int("nx", texWidth);
  compShader.set_int("ny", texHeight);

  unsigned int compShaderLocalSize[] = {(texWidth + 0) / 8, (texHeight + 0) / 8, 1};

  int count = 0;
  while (!glfwWindowShouldClose(window) && count < 10)
  {
    process_input(window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float currentTime = (float)glfwGetTime();
    compShader.use();
    compShader.set_float("time", currentTime);
    compShader.set_float("radius", 2.0f);
    glDispatchCompute(compShaderLocalSize[0], compShaderLocalSize[1], compShaderLocalSize[2]);    
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssboDebug);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, texWidth, texHeight,        // source
                      0, 0, windowWidth, windowHeight,  // destination
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    float debugVal[2];
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 2 * sizeof(float), debugVal);
    std::cout << "Value from GPU: " << debugVal[0] << ", " << debugVal[1] << std::endl;
    count++;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteTextures(1, &texOutput);
  compShader.delete_program();

  glfwTerminate();
  return 0;
}

