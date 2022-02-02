#include "sphere.h"
#include "solvers.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray) {
    // Sphere formula: ||x - position||^2 = r^2
    // Line formula:   x = ray.O + t * ray.D

    Vector L = ray.O - position;
    double a = ray.D.dot(ray.D);
    double b = 2.0 * ray.D.dot(L);
    double c = L.dot(L) - r * r;

    double t0;
    double t1;
    if (not Solvers::quadratic(a, b, c, t0, t1))
        return Hit::NO_HIT();

    // t0 is closest hit
    if (t0 < 0.0)  // check if it is not behind the camera
    {
        t0 = t1;    // try t1
        if (t0 < 0.0) // both behind the camera
            return Hit::NO_HIT();
    }

    // calculate normal
    Point hit = ray.at(t0);
    Vector N = (hit - position).normalized();

    // Note that the direction of the normal is not changed here,
    // but in scene.cpp - if necessary.

    return Hit(t0, N);
}

Vector Sphere::toUV(Point const &hit) {
    // placeholders
    double radians = (angle * PI) / 180;
    Vector vec;
    auto point = hit - position;
    auto axisVector = -radians * axis;
    Point rotatedVec = point;

    // x axis
    rotatedVec.x = point.x;

    const auto cosForX = cos(axisVector.x);
    const auto sinForX = sin(axisVector.x);

    rotatedVec.y = point.y * cosForX - point.z * sinForX;
    rotatedVec.z = point.y * sinForX + point.z * cosForX;

    // y axis
    vec = rotatedVec;

    const auto cosForY = cos(axisVector.y);
    const auto sinForY = sin(axisVector.y);

    rotatedVec.x = vec.x * cosForY + vec.z * sinForY;
    rotatedVec.y = vec.y;
    rotatedVec.z = -vec.x * sinForY + vec.z * cosForY;

    // z axis
    vec = rotatedVec;

    const auto cosForZ = cos(axisVector.z);
    const auto sinForZ = sin(axisVector.z);

    rotatedVec.x = vec.x * cosForZ - vec.y * sinForZ;
    rotatedVec.y = vec.x * sinForZ + vec.y * cosForZ;
    rotatedVec.z = vec.z;

    Point p = rotatedVec;

    double u = 0.5 + ((atan2(p.y, p.x)) / (2 * PI));
    double v = 1.0 - (acos(p.z / r) / PI);

    // Use a Vector to return 2 doubles. The third value is never read.
    return Vector{u, v, 0.0};
}

Sphere::Sphere(Point const &pos, double radius, Vector const &axis, double angle)
    :
// Feel free to modify this constructor.
    position(pos),
    r(radius),
    axis(axis),
    angle(angle) {}
