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

    // GLAD用来管理OpenGL的函数指针，所以在调用任何OpenGL的函数之前我们需要初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

  
    glViewport(
        0, 0,   // position of lower-left corner  
        800,    // width (pixel)
        600     // height (pixel)
    );

    // 需要注册framebuffer_size_callback函数，告诉GLFW希望每当窗口调整大小的时候，调用这个函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /**
     * glfwWindowShouldClose: 检查GLFW是否被要求退出
     * glfwPollEvents: 检查有没有触发什么事件(e.g. 键盘输入，鼠标移动等)、更新窗口状态，调用对应的回调函数
     * glfwSwapBuffers: 交换颜色buffer，a large 2D buffer that contains color values for each pixel in GLFW's window
     *                  that is used to render to during this render iteration and show it as output to the screen
     */
    while (!glfwWindowShouldClose(window))
    {
        // input
        process_input(window);

        // rendering commands here

        // 清空屏幕所用的颜色，用新颜色填充
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
