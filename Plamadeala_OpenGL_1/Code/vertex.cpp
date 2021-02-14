#include "vertex.h"

Vertex createVertex(float x, float y, float z, float r, float g, float b) {
    Vertex newVertex;
    newVertex.x = x;
    newVertex.y = y;
    newVertex.z = z;
    newVertex.r = r;
    newVertex.g = g;
    newVertex.b = b;
    return newVertex;
}