#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::trace(Ray const &ray) {
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx) {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj)
        return Color(0.0, 0.0, 0.0);

    Material material = obj->material;          // the hit objects material
    Point hit = ray.at(min_hit.t);              // the hit point
    Vector N = min_hit.N;                       // the normal at hit point
    Vector V = -ray.D;                          // the view vector

    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector + Vector    vector sum
    *        Vector - Vector    vector difference
    *        Point - Point      yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double * Color     scales each color component (r,g,b)
    *        Color * Color      ditto
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color color = Color(0.0, 0.0, 0.0);

    if (N.dot(V) < 0.0) N *= -1; // ray inside a sphere

    Vector L;
    Vector R;
    Color I_a = material.color * material.ka;
    Color I_d;
    Color I_s;
    for (auto &light : lights) {
        L = (light->position - hit).normalized();
        R = (2 * max(0.0, N.dot(L)) * N - L).normalized();
        I_d = max(0.0, L.dot(N)) * material.color * light->color * material.kd;
        if (N.dot(L) > 0) {
            I_s = pow(max(0.0, R.dot(V)), material.n) * light->color * material.ks;
        } else {
            I_d = Triple(0, 0, 0);
        }
        color += I_d + I_s;
    }

    color += I_a;

    return color;
}

void Scene::render(Image &img) {
    unsigned w = img.width();
    unsigned h = img.height();
    // parallelize the code
#pragma omp parallel for
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x, y) = col;
        }
    }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj) {
    objects.push_back(obj);
}

void Scene::addLight(Light const &light) {
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position) {
    eye = position;
}

unsigned Scene::getNumObject() {
    return objects.size();
}

unsigned Scene::getNumLights() {
    return lights.size();
}
