#include "mesh.h"

#include "../objloader.h"
#include "../vertex.h"
#include "triangle.h"

#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

Hit Mesh::intersect(Ray const &ray) {
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != d_tris.size(); ++idx) {
        Hit hit(d_tris[idx]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = d_tris[idx];
        }
    }
    if (!obj) {
        return Hit::NO_HIT();
    }

    return min_hit;
}

Mesh::Mesh(string const &filename, Point const &position, Vector const &rotation, Vector const &scale) {
    OBJLoader model(filename);
    d_tris.reserve(model.numTriangles());
    vector<Vertex> vertices = model.vertex_data();
    for (size_t tri = 0; tri != model.numTriangles(); ++tri) {
        Vertex one = vertices[tri * 3];
        Point v0(one.x, one.y, one.z);

        Vertex two = vertices[tri * 3 + 1];
        Point v1(two.x, two.y, two.z);

        Vertex three = vertices[tri * 3 + 2];
        Point v2(three.x, three.y, three.z);

        // Apply non-uniform scaling, rotation and translation to the three points
        // of the triangle (v0, v1, and v2) here.

        // Non-uniform scaling
        v0 = v0 * scale;
        v1 = v1 * scale;
        v2 = v2 * scale;

        // Rotation
        Point v0_old = v0;
        Point v1_old = v1;
        Point v2_old = v2;

        // around the x axis
        v0.y = (cos(rotation.x) * v0_old.y - sin(rotation.x) * v0_old.z);
        v0.z = (sin(rotation.x) * v0_old.y + cos(rotation.x) * v0_old.z);

        v1.y = (cos(rotation.x) * v1_old.y - sin(rotation.x) * v1_old.z);
        v1.z = (sin(rotation.x) * v1_old.y + cos(rotation.x) * v1_old.z);

        v2.y = (cos(rotation.x) * v2_old.y - sin(rotation.x) * v2_old.z);
        v2.z = (sin(rotation.x) * v2_old.y + cos(rotation.x) * v2_old.z);

        v0_old = v0;
        v1_old = v1;
        v2_old = v2;

        // around the y axis
        v0.x = (cos(rotation.y) * v0_old.x + sin(rotation.y) * v0_old.z);
        v0.z = (-sin(rotation.y) * v0_old.x + cos(rotation.y) * v0_old.z);

        v1.x = (cos(rotation.y) * v1_old.x + sin(rotation.y) * v1_old.z);
        v1.z = (-sin(rotation.y) * v1_old.x + cos(rotation.y) * v1_old.z);

        v2.x = (cos(rotation.y) * v2_old.x + sin(rotation.y) * v2_old.z);
        v2.z = (-sin(rotation.y) * v2_old.x + cos(rotation.y) * v2_old.z);

        v0_old = v0;
        v1_old = v1;
        v2_old = v2;

        // around the z axis
        v0.x = (cos(rotation.z) * v0_old.x - sin(rotation.z) * v0_old.y);
        v0.y = (sin(rotation.z) * v0_old.x + cos(rotation.z) * v0_old.y);

        v1.x = (cos(rotation.z) * v1_old.x - sin(rotation.z) * v1_old.y);
        v1.y = (sin(rotation.z) * v1_old.x + cos(rotation.z) * v1_old.y);

        v2.x = (cos(rotation.z) * v2_old.x - sin(rotation.z) * v2_old.y);
        v2.y = (sin(rotation.z) * v2_old.x + cos(rotation.z) * v2_old.y);

        // Translation
        v0 = v0 + position;
        v1 = v1 + position;
        v2 = v2 + position;

        d_tris.push_back(ObjectPtr(new Triangle(v0, v1, v2)));
    }

    cout << "Loaded model: " << filename << " with " <<
         model.numTriangles() << " triangles.\n";
}
