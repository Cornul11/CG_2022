#include "sphere.h"

#include <cmath>

using namespace std;

bool solveABC(double &a, double &b, double &c, double &x0, double &x1) {

    double det = pow(b, 2) - (4 * a * c);

    if (det < 0) {
        return false;
    } else if (det == 0) {
        x0 = -(b / (2 * a));
    } else {
        x0 = (-b + sqrt(det)) / (2 * a);
        x1 = (-b - sqrt(det)) / (2 * a);
    }
    return true;
}

Hit Sphere::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (position) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // Intersection calculation
    Vector OC = ray.O - position;
    double t;
    double x0;
    double x1;

    double a = ray.D.dot(ray.D);
    double b = 2 * (ray.D.dot(OC));
    double c = (OC.dot(OC)) - (r * r);

    if (!solveABC(a, b, c, x0, x1)) {
        return Hit::NO_HIT();
    } else {
        if (x0 < 0 && x1 < 0) {
            return Hit::NO_HIT();
        }

        if (x0 > x1 && x1 > 0 && x0 > 0) {
            std::swap(x0, x1);
        }

        t = x0;
    }

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, position, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    Vector hitPoint = ray.O + ray.D * t;
    Vector N = (hitPoint - position).normalized();

    return Hit(t, N);
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
