#include <stdlib.h>
#include <math.h>
#include <cassert>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// --- 1. TRIANGLE (3 vertices) ---
const int TRIANGLE_VERTEX_COUNT = 3;
const int TRIANGLE_INDEX_COUNT = 3;

const float TRIANGLE_POSITIONS[9] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
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