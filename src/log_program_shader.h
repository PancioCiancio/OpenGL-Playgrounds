#pragma once

#include <gl.h>

void LogShaderErrors(GLenum shaderType, GLuint shader)
{
    #ifdef DEBUG
    // Estimated a maximum of 4kb text
    constexpr GLint MAX_SHADER_FILE_SIZE = 1024u * 4u;
    static GLchar INFO_LOG[1024 * 4] = {};

    GLint status = {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    GLint infoLogLength = {};
    glGetShaderInfoLog(shader, MAX_SHADER_FILE_SIZE, &infoLogLength, INFO_LOG);

    if (status == GL_FALSE)
    {
        const char* strShaderType = NULL;
        switch(shaderType)
        {
            case GL_VERTEX_SHADER:
            {
                strShaderType = "Vertex";
                break;
            }
            case GL_GEOMETRY_SHADER:
            {
                strShaderType = "Geometry";
                break;
            }
            case GL_FRAGMENT_SHADER:
            {
                strShaderType = "Fragment";
                break;
            }
        }

        printf("Compile failure in %s shader:\n%s\n", strShaderType, INFO_LOG);
    }
    #endif
}

void LogProgramErrors(GLuint program)
{
    #ifdef DEBUG
    // Estimated a maximum of 4kb text
    constexpr GLint MAX_PROGRAM_FILE_SIZE = 1024u * 4u;
    static GLchar INFO_LOG[1024 * 4] = {};

    GLint status = {};
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint infoLogLength = {};
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        glGetProgramInfoLog(program, MAX_PROGRAM_FILE_SIZE, &infoLogLength, INFO_LOG);
        printf("[OpenGL] Linker failure: %s\n", INFO_LOG);
    }
    #endif
}