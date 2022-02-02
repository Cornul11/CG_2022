#include "triangle.h"

Hit Triangle::intersect(Ray const &ray) {
    // as seen on page 79
    Vector small_a = v0;
    Vector small_b = v1;
    Vector small_c = v2;
    Vector small_d = ray.D;
    Vector small_e = ray.O;
    double a = small_a.x - small_b.x;
    double b = small_a.y - small_b.y;
    double c = small_a.z - small_b.z;
    double d = small_a.x - small_c.x;
    double e = small_a.y - small_c.y;
    double f = small_a.z - small_c.z;
    double g = small_d.x;
    double h = small_d.y;
    double i = small_d.z;
    double j = small_a.x - small_e.x;
    double k = small_a.y - small_e.y;
    double l = small_a.z - small_e.z;

    double M = a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
    double t = -((f * (a * k - j * b) + e * (j * c - a * l) + d * (b * l - k * c)) / M);

    if (t < 0) {
        return Hit::NO_HIT();
    }

    double gamma = (i * (a * k - j * b) + h * (j * c - a * l) + g * (b * l - k * c)) / M;

    if (gamma < 0 || gamma > 1) {
        return Hit::NO_HIT();
    }

    double beta = (j * (e * i - h * f) + k * (g * f - d * i) + l * (d * h - e * g)) / M;

    if (beta < 0 || beta > 1 - gamma) {
        return Hit::NO_HIT();
    }

    if (N.dot(ray.D) > 0) {
        return {t, -1 * N};
    } else return {t, N};
}

Triangle::Triangle(Point const &v0,
                   Point const &v1,
                   Point const &v2)
    :
    v0(v0),
    v1(v1),
    v2(v2),
    N() {
    // Calculate the surface normal and store it in N
    N = (v1 - v0).cross(v2 - v0).normalized();
}
