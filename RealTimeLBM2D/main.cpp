#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLHelpers.h"
#include "LBMHelpers.h"
#include "Shader.h"
#include "ComputeShader.h"

struct PingPongBuffer
{
private:
  unsigned int bufferIDs[2];
  int writeIdx = 1;
public:
  PingPongBuffer(const unsigned int& bufferRead, const unsigned int& bufferWrite)
  {
    bufferIDs[0] = bufferRead;
    bufferIDs[1] = bufferWrite;
  }
  void swap() { writeIdx = 1 - writeIdx; }
  unsigned int get_reader() { return bufferIDs[1 - writeIdx]; }
  unsigned int get_writer() { return bufferIDs[writeIdx]; }
};


int main()
{
  unsigned int windowWidth = 800;
  unsigned int windowHeight = 800;
  std::string windowTitle = "RealTimeLBM2D";
  auto window = gl_init_window(windowWidth, windowHeight, windowTitle);

  // output texture
  unsigned int texOutput;
  unsigned int texWidth = 8;
  unsigned int texHeight = 8;
  gl_init_texture(texOutput, texWidth + 2, texHeight + 2, GL_TEXTURE0);

  unsigned int fboOutput;
  glGenFramebuffers(1, &fboOutput);
  glBindFramebuffer(GL_FRAMEBUFFER, fboOutput);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texOutput, 0);

  // obstacles
  unsigned int texObstacle;
  unsigned int fboObstacle;
  gl_init_obstacle_tex_fbo(texObstacle, fboObstacle, texWidth + 2, texHeight + 2);
  //! tmp obstacle
  float vertices[] = {
    0.4f * (float)texWidth, 0.4f * (float)texHeight, 0.0f,
    0.6f * (float)texWidth, 0.4f * (float)texHeight, 0.0f,
    0.5f * (float)texWidth, 0.6f * (float)texHeight, 0.0f
  };
  std::vector<int> attribComps(1, 3); // only 1 attribute (position), stride = 3
  unsigned int vaoObstacle;
  unsigned int vboObstacle;
  gl_init_vao_vbo(vaoObstacle, vboObstacle, attribComps, sizeof(vertices), vertices);
  //! tmp obstacle

  // domain boundaries mask
  // unsigned int texBoundaryMask;
  // std::vector<uint16_t> boundaryMask = build_boundary_mask(texWidth, texHeight);
  // gl_init_mask_texture(texBoundaryMask, boundaryMask.data(), texWidth, texHeight, GL_TEXTURE1);

  //!!!!!!!! should not use a texture only for boundaries: fetching texture memory is much slower than math calculations
  //!!!!!!!!!! obstacle and domain boundaries share one texture ?

  // pdf && initialization
  int strideSoA = (texWidth + 2) * (texHeight + 2);
  unsigned int ssboPdfIn;
  glGenBuffers(1, &ssboPdfIn);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPdfIn);
  std::vector<float> pdfIn(strideSoA * 9, 0.0f);
  std::vector<float> initVel{0.0f, 0.0f};
  for (int i = 0; i < 9; ++i)
  {
    float feq = compute_feq(i, 1.0f, initVel[0], initVel[1]);
    std::fill(pdfIn.begin() + i * strideSoA, pdfIn.begin() + (i + 1) * strideSoA, feq);
  }
  glBufferData(GL_SHADER_STORAGE_BUFFER, strideSoA * 9 * sizeof(float), pdfIn.data(), GL_DYNAMIC_DRAW);
  // glBufferStorage(GL_SHADER_STORAGE_BUFFER, strideSoA * 9 * sizeof(float), pdfIn.data(), GL_DYNAMIC_STORAGE_BIT);

  unsigned int ssboPdfOut;
  glGenBuffers(1, &ssboPdfOut);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPdfOut);
  glBufferData(GL_SHADER_STORAGE_BUFFER, strideSoA * 9 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
  // glBufferStorage(GL_SHADER_STORAGE_BUFFER, strideSoA * 9 * sizeof(float), nullptr, GL_DYNAMIC_STORAGE_BIT);

  // shaders
  Shader obstacleShader("../obstacle.vs", "../obstacle.fs");
  // collision & streaming compute shader
  ComputeShader collideStreamShader("../collideStream.comp");
  float kinematicViscosity = 0.01f;
  float omega = 1.0f / (3.0f * kinematicViscosity + 0.5f);
  collideStreamShader.set_int("nx", texWidth);
  collideStreamShader.set_int("ny", texHeight);
  collideStreamShader.set_int("strideSoA", strideSoA);
  collideStreamShader.set_float("omega", omega);

  // boundary condition shader
  ComputeShader bcShader("../boundaryCondition.comp");
  bcShader.set_int("nx", texWidth);
  bcShader.set_int("ny", texHeight);
  bcShader.set_int("strideSoA", strideSoA);
  bcShader.set_float("omega", omega);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboPdfIn);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboPdfOut);

  unsigned int compShaderLocalSize[] = {(texWidth + 2) / 16, (texHeight + 2) / 16, 1};
  PingPongBuffer pdfBuffer(ssboPdfIn, ssboPdfOut);

  unsigned int queries[2];
  glGenQueries(2, queries);

  // debug buffer
  unsigned int ssboDebug;
  glGenBuffers(1, &ssboDebug);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDebug);
  glBufferData(GL_SHADER_STORAGE_BUFFER, 9 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

  int count = 0;
  while (!glfwWindowShouldClose(window) && count < 10)
  {
    process_input(window);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float currentTime = (float)glfwGetTime();
    glQueryCounter(queries[0], GL_TIMESTAMP);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pdfBuffer.get_reader());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pdfBuffer.get_writer());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, ssboDebug);

    glBindVertexArray(vaoObstacle);
    obstacleShader.use();
    glm::mat4 model(1.0f);
    // model = glm::translate(model, glm::vec3((float)texWidth * 0.5f, (float)texHeight * 0.5f, 0.0f));
    // model = glm::rotate(model, glm::radians(20.0f) * currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
    // model = glm::translate(model, -glm::vec3((float)texWidth * 0.5f, (float)texHeight * 0.5f, 0.0f));
    glm::mat4 projection = glm::ortho(0.0f, (float)texWidth, 0.0f, (float)texHeight);
    obstacleShader.set_mat4("model", model);
    obstacleShader.set_mat4("projection", projection);

    collideStreamShader.use();
    glDispatchCompute(compShaderLocalSize[0], compShaderLocalSize[1], compShaderLocalSize[2]);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboDebug);
    float debugVal[9];
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 9 * sizeof(float), debugVal);
    std::cout << "step=" << count << std::endl;
    std::cout << "f = ";
    for (int i = 0; i < 9; ++i)
    {
      printf("%.4f    ", debugVal[i]);
    }
    std::cout << std::endl;

    // bcShader.use();
    // bcShader.set_float("time", currentTime);
    // glDispatchCompute(compShaderLocalSize[0], compShaderLocalSize[1], compShaderLocalSize[2]);
    // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboOutput); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, texWidth, texHeight,
                      0, 0, windowWidth, windowHeight,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    pdfBuffer.swap();

    glQueryCounter(queries[1], GL_TIMESTAMP);
    GLuint64 timeStart, timeEnd;
    glGetQueryObjectui64v(queries[0], GL_QUERY_RESULT, &timeStart);
    glGetQueryObjectui64v(queries[1], GL_QUERY_RESULT, &timeEnd);
    double durationSeconds = (timeEnd - timeStart) / 10e9;
    // printf("Render Time: %.6f s\n", durationSeconds);

    count++;
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

