#include <stdlib.h>
#include <math.h>
#include <cassert>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Structure of Arrays (SoA) with Index Support
struct MeshData 
{
    int vertexCount;
    int indexCount;
    
    float* positions;      // Size: vertexCount * 3
    float* normals;        // Size: vertexCount * 3
    float* uvs;            // Size: vertexCount * 2
    unsigned int* indices; // Size: indexCount
};

// Explicit memory allocation
MeshData AllocateMesh(int vertexCount, int indexCount) 
{
    MeshData mesh = {};
    mesh.vertexCount = vertexCount;
    mesh.indexCount  = indexCount;
    
    // @todo: generate one heap allocation and align element in the same heap.
    // Maybe just use a static geometries memeory buffer.
    mesh.positions   = (float*)malloc(vertexCount * 3 * sizeof(float));
    mesh.normals     = (float*)malloc(vertexCount * 3 * sizeof(float));
    mesh.uvs         = (float*)malloc(vertexCount * 2 * sizeof(float));
    mesh.indices     = (unsigned int*)malloc(indexCount * sizeof(unsigned int));
    
    return mesh;
}

// Explicit memory cleanup
void FreeMesh(MeshData* mesh) 
{
    assert(mesh);

    if (mesh->positions) free(mesh->positions);
    if (mesh->normals)   free(mesh->normals);
    if (mesh->uvs)       free(mesh->uvs);
    if (mesh->indices)   free(mesh->indices);
    
    mesh->positions = NULL;
    mesh->normals   = NULL;
    mesh->uvs       = NULL;
    mesh->indices   = NULL;
    mesh->vertexCount = 0;
    mesh->indexCount  = 0;
}

// --- 1. TRIANGLE (3 vertices) ---
const int TRIANGLE_VERTEX_COUNT = 3;
const int TRIANGLE_INDEX_COUNT = 3;

const float TRIANGLE_POSITIONS[9] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

const float TRIANGLE_NORMALS[9] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};

const float TRIANGLE_UVS[6] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
};

const unsigned int TRIANGLE_INDICES[6] = {
    0, 1, 2
};

// --- QUAD (4 Vertices, 6 Indices) ---
const int QUAD_VERTEX_COUNT = 4;
const int QUAD_INDEX_COUNT  = 6;

const float QUAD_POSITIONS[12] = {
    -0.5f, -0.5f, 0.0f, // 0: Bottom-Left
     0.5f, -0.5f, 0.0f, // 1: Bottom-Right
     0.5f,  0.5f, 0.0f, // 2: Top-Right
    -0.5f,  0.5f, 0.0f  // 3: Top-Left
};

const float QUAD_NORMALS[12] = {
    0.0f, 0.0f, 1.0f,   // 0
    0.0f, 0.0f, 1.0f,   // 1
    0.0f, 0.0f, 1.0f,   // 2
    0.0f, 0.0f, 1.0f    // 3
};

const float QUAD_UVS[8] = {
    0.0f, 0.0f,         // 0
    1.0f, 0.0f,         // 1
    1.0f, 1.0f,         // 2
    0.0f, 1.0f          // 3
};

// Counter-clockwise winding order
const unsigned int QUAD_INDICES[6] = {
    0, 1, 2,  // Triangle 1
    2, 3, 0   // Triangle 2
};

// Note: The Cube follows this exact same pattern (36 vertices). 
// You would have 108 floats for positions, 108 for normals, and 72 for UVs.
const int CUBE_VERTEX_COUNT = 24;
const int CUBE_INDEX_COUNT  = 36;

// 24 Vertices * 3 floats = 72 floats
const float CUBE_POSITIONS[72] = {
    // Front Face (Z = 0.5)
    -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,
    // Back Face (Z = -0.5) - Note: X is flipped so CCW faces outward
     0.5f, -0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
    // Left Face (X = -0.5)
    -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,
    // Right Face (X = 0.5)
     0.5f, -0.5f,  0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f,
    // Top Face (Y = 0.5)
    -0.5f,  0.5f,  0.5f,   0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
    // Bottom Face (Y = -0.5)
    -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f
};

// 24 Vertices * 3 floats = 72 floats
const float CUBE_NORMALS[72] = {
    // Front
     0.0f,  0.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f,  0.0f,  1.0f,
    // Back
     0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f,  0.0f, -1.0f,
    // Left
    -1.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,
    // Right
     1.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,   1.0f,  0.0f,  0.0f,
    // Top
     0.0f,  1.0f,  0.0f,   0.0f,  1.0f,  0.0f,   0.0f,  1.0f,  0.0f,   0.0f,  1.0f,  0.0f,
    // Bottom
     0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f,   0.0f, -1.0f,  0.0f
};

// 24 Vertices * 2 floats = 48 floats
// The UVs are exactly the same for every face!
const float CUBE_UVS[48] = {
    // Front
    0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
    // Back
    0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
    // Left
    0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
    // Right
    0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
    // Top
    0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
    // Bottom
    0.0f, 0.0f,   1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f
};

// 6 Faces * 2 Triangles * 3 Indices = 36 Indices
// The pattern (0, 1, 2, 2, 3, 0) repeats, just offset by 4 for each face.
const unsigned int CUBE_INDICES[36] = {
    0,  1,  2,      2,  3,  0,      // Front
    4,  5,  6,      6,  7,  4,      // Back
    8,  9,  10,     10, 11, 8,      // Left
    12, 13, 14,     14, 15, 12,     // Right
    16, 17, 18,     18, 19, 16,     // Top
    20, 21, 22,     22, 23, 20      // Bottom
};