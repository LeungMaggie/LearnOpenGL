#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OS X system
  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
  }


  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // ======================================================================


  /*
    Modern OpenGL requires that we at least set up
    a vertex and fragment shader if we want to do some rendering
  */

  //* Vertex shader

  // GLSL language source code
  const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // attach the shader source code to the shader object and compile the shader
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //1: how many strings we'are passing as source code
  glCompileShader(vertexShader);

  // check if compilation was successful
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  //* Fragment shader

  // declare output values with the "out" keyword (FragColor here)
  // orange color with alpha 1.0 (completely opaque)
  const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    /*
      next: link both shaders to a shader program object
      then active this shader program when rendering object.
    */
    //* Shader program
    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check if linking a shader program failed
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //* Linking vertex attributes
    /* 
      tell OpenGL how to interpret the vertex data before rendering
      and how it should connect the vertex data to the vertex shader's attributes
    */
  //  glVertexAttribPointer(
  //   0,                  // specifies which vertex attribute we want to configure (layout (location = 0))
  //   3,                  // size of the vertex attribute (vec3)
  //   GL_FLOAT,           // data type
  //   GL_FALSE,           // if we want the data to be normalized
  //   3 * sizeof(float),  // space between consecutive vertex attributes
  //   (void*)0            // offset of where the position data begins in the buffer
  // );

  // have to repeat this process every time we want to draw an object (not practical!)
  // // 0. copy vertices array in a buffer
  // glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // // 1. set the vertex attributes pointers
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // glEnableVertexAttribArray(0);
  // // 2. use shader program when we want to render an object
  // glUseProgram(shaderProgram);
  // // 3. draw the object
  // // someOpenGLFunctionThatDrawsOurTriangle();
  // vertices of an triangle: z==0.0f such that depths are the same
  // in the range of [-1, 1]?
  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
  };

  unsigned int VBO;
  glGenBuffers(1, &VBO); // 生成带有buffer ID的VBO对象
  glBindBuffer(GL_ARRAY_BUFFER, VBO); // 把创建的buffer绑定到GL_ARRAY_BUFFER目标上
  /**
   * GL_STATIC_DRAW ：数据不会或几乎不会改变
   * GL_DYNAMIC_DRAW：数据会被改变很多
   * GL_STREAM_DRAW ：数据每次绘制时都会改变
   */
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把用户定义的数据复制到当前绑定buffer
  // 现在已经完成把顶点数据储存在显卡的内存中，用VBO这个顶点缓冲对象管理

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);

  // 1. 绑定VAO
  glBindVertexArray(VAO);
  // 2. 把顶点数组复制到buffer中
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // 3. 设置顶点属性指针
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0); //!!!
  // ======================================================================

  while (!glfwWindowShouldClose(window))
  {
    process_input(window);

    // rendering commands here

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

  // ======================================================================
    // 4. 绘制物体
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    // someOpenGLFunctionThatDrawOurTriangle();
    glDrawArrays(
      GL_TRIANGLES, // 希望绘制的是三角形
      0,            // 指定顶点数组的起始索引
      3             // 打算绘制多少个顶点
    );
  // ======================================================================

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
