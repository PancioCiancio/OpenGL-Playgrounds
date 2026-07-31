// System libraries
#include <cstdio>
#include <chrono>
#include <thread>

#include "shaders/triangle.h"
#include "geometries/primitives.h"
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

struct alignas(64) SharedStorageBuffer
{
    float ModelMat[16] = {};
};

const SharedStorageBuffer SSBO[] = {
    {   
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f  
    },
    {   
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.8f, 0.0f, 0.0f, 1.0f  
    },
};

struct DrawElementsIndirectCommand 
{
    GLuint count            = {}; // Number of indices to draw
    GLuint instanceCount    = {}; // Number of instances to draw
    GLuint firstIndex       = {}; // Offset into index buffer
    GLuint baseVertex       = {}; // Offset into vertex buffer
    GLuint baseInstance     = {}; // Offset for gl_InstanceID
};

constexpr size_t MAX_VERTEX_BUFFER_SIZE = 1024 * 1024 * 4;  // 4mb
constexpr size_t MAX_INDEX_BUFFER_SIZE = 1024 * 1024 * 4;   // 4mb
constexpr size_t MAX_INSTANCES = 10000;

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

    GLuint vao              = {};
    GLuint vboPos           = {};
    GLuint vboNorm          = {};
    GLuint vboUV            = {};
    GLuint ebo              = {};
    GLuint ssbo             = {};
    GLuint indirectBuffer   = {};

    glCreateBuffers(1, &vboPos);
    glNamedBufferStorage(vboPos, MAX_VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferData(vboPos, sizeof(QUAD_POSITIONS), QUAD_POSITIONS, GL_STATIC_DRAW);

    glCreateBuffers(1, &vboNorm);
    glNamedBufferStorage(vboNorm, MAX_VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferData(vboNorm, sizeof(QUAD_NORMALS), QUAD_NORMALS, GL_STATIC_DRAW);

    glCreateBuffers(1, &vboUV);
    glNamedBufferStorage(vboUV, MAX_VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferData(vboUV, sizeof(QUAD_UVS), QUAD_UVS, GL_STATIC_DRAW);

    glCreateBuffers(1, &ebo);
    glNamedBufferStorage(ebo, MAX_INDEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferData(ebo, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

    glCreateBuffers(1, &ssbo);
    glNamedBufferStorage(ssbo, MAX_INSTANCES * sizeof(SharedStorageBuffer), nullptr, GL_DYNAMIC_STORAGE_BIT);
    // glNamedBufferData(ssbo, sizeof(SSBO), SSBO, GL_STATIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // 2. Direct VAO Setup
    glCreateVertexArrays(1, &vao);
    glVertexArrayElementBuffer(vao, ebo); // Directly attach EBO to VAO

    // --- Position (Location 0) ---
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0); 
    glVertexArrayAttribBinding(vao, 0, 0); // Link attribute 0 to VAO binding point 0
    glVertexArrayVertexBuffer(vao, 0, vboPos, 0, sizeof(float) * 3); // Attach vboPos to VAO binding point 0

    // --- Normal (Location 1) ---
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 1, 1); // Link attribute 1 to VAO binding point 1
    glVertexArrayVertexBuffer(vao, 1, vboNorm, 0, sizeof(float) * 3);

    // --- UV (Location 2) ---
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 2, 2); // Link attribute 2 to VAO binding point 2
    glVertexArrayVertexBuffer(vao, 2, vboUV, 0, sizeof(float) * 2);

    // 3. Indirect Command Buffer Setup
    DrawElementsIndirectCommand cmd[] = {
        {6, 1, 0, 0, 0},
        {3, 1, 6, 4, 1}};
    glCreateBuffers(1, &indirectBuffer);
    glNamedBufferData(indirectBuffer, sizeof(cmd), &cmd, GL_STATIC_DRAW);

    // Allocate one quad
    glNamedBufferSubData(vboPos,    0, sizeof(QUAD_POSITIONS),  QUAD_POSITIONS);
    glNamedBufferSubData(vboNorm,   0, sizeof(QUAD_NORMALS),    QUAD_NORMALS);
    glNamedBufferSubData(vboUV,     0, sizeof(QUAD_UVS),        QUAD_UVS);
    glNamedBufferSubData(ebo,       0, sizeof(QUAD_INDICES),    QUAD_INDICES);

    // Allocate one triangle
    glNamedBufferSubData(vboPos,    sizeof(QUAD_POSITIONS), sizeof(TRIANGLE_POSITIONS),  TRIANGLE_POSITIONS);
    glNamedBufferSubData(vboNorm,   sizeof(QUAD_NORMALS),   sizeof(TRIANGLE_NORMALS),    TRIANGLE_NORMALS);
    glNamedBufferSubData(vboUV,     sizeof(QUAD_UVS),       sizeof(TRIANGLE_UVS),        TRIANGLE_UVS);
    glNamedBufferSubData(ebo,       sizeof(QUAD_INDICES),   sizeof(TRIANGLE_INDICES),    TRIANGLE_INDICES);

    glNamedBufferSubData(ssbo, 0, sizeof(SSBO), SSBO);

    #ifdef DEBUG
    // Define debug symbol that can be red from capture tools like RenderDoc or NVidiaNsight
    const char* vboPosName = "VBO_Positions";
    glObjectLabel(GL_BUFFER, vboPos, -1, vboPosName);

    const char* vboNormName = "VBO_Normals";
    glObjectLabel(GL_BUFFER, vboNorm, -1, vboNormName);

    const char* vboUVsName = "VBO_UVs";
    glObjectLabel(GL_BUFFER, vboUV, -1, vboUVsName);

    const char* eboName = "EBO_Indices";
    glObjectLabel(GL_BUFFER, ebo, -1, eboName);

    const char* ssboName = "SSBO_InstanceData";
    glObjectLabel(GL_BUFFER, ssbo, -1, ssboName);
    #endif

    // Vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &Shader::Triangle::VERTEX, NULL);
    glCompileShader(vertexShader);
    LogShaderErrors(GL_VERTEX_SHADER, vertexShader);

    // Fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &Shader::Triangle::FRAGMENT, NULL);
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

    // Loop
    constexpr double TARGET_FPS = 60.0;
    constexpr double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;


    while(!glfwWindowShouldClose(window))
    {
        double frameStartTime = glfwGetTime();

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
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 2, sizeof(DrawElementsIndirectCommand));

        // keep running
        glfwSwapBuffers(window);


        double frameEndTime = glfwGetTime();
        double timeTaken = frameEndTime - frameStartTime;

        if (timeTaken < TARGET_FRAME_TIME)
        {
            double timeToSleep = TARGET_FRAME_TIME - timeTaken;
            std::this_thread::sleep_for(std::chrono::duration<double>(timeToSleep));
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
}