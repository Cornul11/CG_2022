#include "quad.h"
#include "triangle.h"

#include <limits>

using namespace std;

Hit Quad::intersect(Ray const &ray) {
    Hit min_hit(numeric_limits<double>::infinity(), Vector());

    Hit first_triangle_hit = first_triangle->intersect(ray);
    Hit second_triangle_hit = second_triangle->intersect(ray);

    if (first_triangle_hit.t < min_hit.t) {
        min_hit = first_triangle_hit;
    }
    if (second_triangle_hit.t < min_hit.t) {
        min_hit = second_triangle_hit;
    }

    if (min_hit.t < numeric_limits<double>::infinity()) {
        return min_hit;
    }
    return Hit::NO_HIT();
}

Quad::Quad(Point const &v0,
           Point const &v1,
           Point const &v2,
           Point const &v3) {
    first_triangle = new Triangle(v0, v1, v2);
    second_triangle = new Triangle(v0, v2, v3);
}
