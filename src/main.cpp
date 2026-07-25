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

const char* VERTEX_SHADER_TEXT =
"#version 330\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
const char* FRAGMENT_SHADER_TEXT =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

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

    GLuint vertexBuffer = {};
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER_TEXT, NULL);
    glCompileShader(vertexShader);

    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_TEXT, NULL);
    glCompileShader(fragmentShader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    const GLint vposLocation = glGetAttribLocation(program, "vPos");
    const GLint vcolLocation = glGetAttribLocation(program, "vCol");

    GLuint vertexArray = {};
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glEnableVertexAttribArray(vposLocation);
    glVertexAttribPointer(vposLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));
    glEnableVertexAttribArray(vcolLocation);
    glVertexAttribPointer(vcolLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Col));


    while(!glfwWindowShouldClose(window))
    {

        int width = {};
        int height = {};

        glfwGetFramebufferSize(window, &width, &height);

        const float ratio = (float)width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // keep running
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}