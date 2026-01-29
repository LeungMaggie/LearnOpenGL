#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLHelpers.h"
#include "LBMHelpers.h"
#include "Shader.h"
#include "ComputeShader.h"

int main()
{
  unsigned int windowWidth = 800;
  unsigned int windowHeight = 800;
  std::string windowTitle = "RealTimeLBM2D";
  auto window = gl_init_window(windowWidth, windowHeight, windowTitle);

  // output texture
  unsigned int texOutput;
  unsigned int texWidth = 128;
  unsigned int texHeight = 128;
  gl_init_texture(texOutput, texWidth, texHeight, GL_TEXTURE0);

  unsigned int fboOutput;
  glGenFramebuffers(1, &fboOutput);
  glBindFramebuffer(GL_FRAMEBUFFER, fboOutput);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texOutput, 0);

  // obstacles
  unsigned int texObstacle;
  unsigned int fboObstacle;
  gl_init_obstable_tex_fbo(texObstacle, fboObstacle, texWidth, texHeight);
  //! tmp obstacle
  float vertices[] = {
    0.4f, 0.4f, 0.0f,
    0.6f, 0.4f, 0.0f,
    0.5f, 0.6f, 0.0f
  };
  std::vector<int> attribComps(1, 3); // only 1 attribute (position), stride = 3
  unsigned int vaoObstacle;
  unsigned int vboObstacle;
  gl_init_vao_vbo(vaoObstacle, vboObstacle, attribComps, sizeof(vertices), vertices);
  //! tmp obstacle

  // domain boundaries mask
  unsigned int texBoundaryMask;
  std::vector<uint16_t> boundaryMask = build_boundary_mask(texWidth, texHeight);
  gl_init_mask_texture(texBoundaryMask, boundaryMask.data(), texWidth, texHeight, GL_TEXTURE1);

  //!!!!!!!! should not use a texture only for boundaries: fetching texture memory is much slower than math calculations
  //!!!!!!!!!! obstacle and domain boundaries share one texture ?

  // shaders
  Shader obstacleShader("../obstacle.vs", "../obstacle.fs");
  ComputeShader collideStreamShader("../collideStream.comp");
  ComputeShader bcShader("../boundaryCondition.comp");
  unsigned int compShaderLocalSize[] = {texWidth / 16, texHeight / 16, 1};

  while (!glfwWindowShouldClose(window))
  {
    process_input(window);
    float currentTime = (float)glfwGetTime();

    glBindVertexArray(vaoObstacle);

    obstacleShader.use();
    // set mat4 "model", "projection"

    collideStreamShader.use();
    glDispatchCompute(compShaderLocalSize[0], compShaderLocalSize[1], compShaderLocalSize[2]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    bcShader.use();
    bcShader.set_float("time", currentTime);
    glDispatchCompute(compShaderLocalSize[0], compShaderLocalSize[1], compShaderLocalSize[2]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboOutput); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, texWidth, texHeight,
                      0, 0, texWidth, texHeight,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glDeleteTextures(1, &texOutput);
  obstacleShader.delete_program();
  collideStreamShader.delete_program();
  bcShader.delete_program();

  glfwTerminate();
  return 0;
}

