#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLHelpers.h"
#include "Shader.h"
#include "ComputeShader.h"

int main()
{
  unsigned int windowWidth = 800;
  unsigned int windowHeight = 600;
  std::string windowTitle = "RealTimeLBM2D";
  auto window = gl_init_window(windowWidth, windowHeight, windowTitle);

  ComputeShader compShader("../circle.comp");

  unsigned int texOutput;
  unsigned int texWidth = windowWidth;
  unsigned int texHeight = windowHeight;
  gl_init_texture(texOutput, texWidth, texHeight, GL_TEXTURE0);

  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texOutput, 0);

  unsigned int compShaderLocalSize[] = {texWidth / 10, texHeight / 10, 1};

  while (!glfwWindowShouldClose(window))
  {
    process_input(window);
    float currentTime = (float)glfwGetTime();
    compShader.use();
    compShader.set_float("time", currentTime);
    compShader.set_float("radius", 100.0f);
    glDispatchCompute(compShaderLocalSize[0], compShaderLocalSize[1], compShaderLocalSize[2]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, texWidth, texHeight,
                      0, 0, texWidth, texHeight,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteTextures(1, &texOutput);
  compShader.delete_program();

  glfwTerminate();
  return 0;
}

