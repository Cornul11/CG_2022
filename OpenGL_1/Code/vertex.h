#ifndef VERTEX_H
#define VERTEX_H

typedef struct Vertex {
    float x, y, z;
    float r, g, b;
} Vertex;

Vertex createVertex(float x, float y, float z, float r, float g, float b);

#endif //VERTEX_H