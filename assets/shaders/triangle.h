#pragma once

#include <string>

namespace Shader { namespace Triangle {
    
    const char* VERTEX = R"(
    #version 450 core
    #extension GL_ARB_shader_draw_parameters : require // Add this extension

    struct InstanceData
    {
        mat4 modelMat;
    };

    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec3 vNorm;
    layout(location = 2) in vec2 vUVs;
    layout(std430, binding = 0) buffer InstanceBuffer
    {
        InstanceData instances[];
    };

    out flat int vInstanceID;
    out vec3 color;
    
    void main()
    {
        int actualInstanceIndex = gl_InstanceID + gl_BaseInstanceARB;
        
        gl_Position = instances[actualInstanceIndex].modelMat * vec4(vPos, 1.0);
        vInstanceID = actualInstanceIndex;
        color = vec3(vUVs, 0.0);
    }
    )";


    const char* FRAGMENT = R"(
    #version 450 core
    #extension GL_ARB_shader_draw_parameters : require // Add this extension

    in flat int vInstanceID;
    in vec3 color;
    out vec4 fragment;

    void main()
    {
        fragment = vec4(color, 1.0);
    }
    )";
}}