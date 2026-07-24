// System libraries
#include <cstdio>

// Graphics libraries
#define GLFW_INCLUDE_NONE
#include <glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <gl.h>


void GLFW_ERROR_CALLBACK(int error, const char* description)
{
    printf("[glfw] ERROR: %s\n", description);
}

void GLFW_KEY_CALLBACK(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


int main()
{
    if (!glfwInit())
    {
        printf("[glfw] ERROR: initalization failed");
    }

    glfwSetErrorCallback(GLFW_ERROR_CALLBACK);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(640, 400, "Image Processing", NULL, NULL);

    if (!window)
    {
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, GLFW_KEY_CALLBACK);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window))
    {
        // keep running
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}