// System libraries
#include <cstdio>

#include "log_program_shader.h"

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


typedef struct Vertex
{
    float Pos[2];
    float Col[3];
} Vertex;

const Vertex VERTICES[3] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

const uint32_t INDICES[] = {0, 1, 2};

const char* VERTEX_SHADER_TEXT =
"#version 450 core\n"
"layout(location = 0) in vec2 aPos;\n"
"layout(location = 1) in vec3 vCol;\n"
"out flat int vInstanceID;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"    vInstanceID = gl_InstanceID;\n"
"    color = vCol;\n"
"}\n";
 
const char* FRAGMENT_SHADER_TEXT =
"#version 450 core\n"
"in flat int vInstanceID;\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

// 1. The standard Indirect Draw Command struct
struct DrawElementsIndirectCommand {
    GLuint count;         // Number of indices to draw
    GLuint instanceCount; // Number of instances to draw
    GLuint firstIndex;    // Offset into index buffer
    GLuint baseVertex;    // Offset into vertex buffer
    GLuint baseInstance;  // Offset for gl_InstanceID
};

int main()
{
    if (!glfwInit())
    {
        printf("[glfw] ERROR: initalization failed");
    }

    glfwSetErrorCallback(GLFW_ERROR_CALLBACK);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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

    GLuint vao = {};
    GLuint vbo = {};
    GLuint ebo = {};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

    // Vertex attributes (pos and color)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Col));

    DrawElementsIndirectCommand cmd = {};
    cmd.count           = 3;
    cmd.instanceCount   = 1;
    cmd.firstIndex      = 0;
    cmd.baseVertex      = 0;
    cmd.baseInstance    = 0;

    GLuint indirectBuffer = {};
    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand), &cmd, GL_STATIC_DRAW);

    // Vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER_TEXT, NULL);
    glCompileShader(vertexShader);
    LogShaderErrors(GL_VERTEX_SHADER, vertexShader);

    // Fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_TEXT, NULL);
    glCompileShader(fragmentShader);
    LogShaderErrors(GL_FRAGMENT_SHADER, fragmentShader);

    // Assembly program
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    LogProgramErrors(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width = {};
        int height = {};

        glfwGetFramebufferSize(window, &width, &height);

        const float ratio = (float)width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 1, sizeof(DrawElementsIndirectCommand));

        // keep running
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}