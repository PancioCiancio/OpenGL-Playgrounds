#pragma once

#include <string>

namespace Shader { namespace Triangle {
    
    const char* VERTEX = R"(
    #version 450 core
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
        gl_Position = instances[gl_InstanceID].modelMat * vec4(vPos, 1.0);

        vInstanceID = gl_InstanceID;

        color = vNorm;
    }
    )";


    const char* FRAGMENT = R"(
    #version 450 core
    in flat int vInstanceID;
    in vec3 color;
    out vec4 fragment;

    void main()
    {
        fragment = vec4(color, 1.0);
    }
    )";
}}