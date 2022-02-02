#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

pair<ObjectPtr, Hit> Scene::castRay(Ray const &ray) const {
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

    return pair<ObjectPtr, Hit>(obj, min_hit);
}

Color Scene::trace(Ray const &ray, unsigned depth) {
    pair<ObjectPtr, Hit> mainhit = castRay(ray);
    ObjectPtr obj = mainhit.first;
    Hit min_hit = mainhit.second;

    // No hit? Return background color.
    if (!obj)
        return Color(0.0, 0.0, 0.0);

    Material const &material = obj->material;
    Point hit = ray.at(min_hit.t);
    Vector V = -ray.D;

    // Pre-condition: For closed objects, N points outwards.
    Vector N = min_hit.N;

    // The shading normal always points in the direction of the view,
    // as required by the Phong illumination model.
    Vector shadingN;
    if (N.dot(V) >= 0.0)
        shadingN = N;
    else
        shadingN = -N;

    Color matColor = material.color;

    if (material.hasTexture) {
        Point p = obj->toUV(hit);
        matColor = material.texture.colorAt(p.x, 1 - p.y);
    }

    // Add ambient once, regardless of the number of lights.
    Color color = material.ka * matColor;

    // Add diffuse and specular components.
    for (auto const &light : lights) {
        Vector L = (light->position - hit).normalized();

        //Render shadows
        if (renderShadows) {
            Ray shadow(hit + (epsilon * shadingN), L);
            pair<ObjectPtr, Hit> s_hit = castRay(shadow);
            if (s_hit.second.t < (light->position - hit).length()) {
                continue;
            }
        }

        // Add diffuse.
        double dotNormal = shadingN.dot(L);
        double diffuse = std::max(dotNormal, 0.0);
        color += diffuse * material.kd * light->color * matColor;

        // Add specular.
        if (dotNormal > 0) {
            Vector reflectDir = reflect(-L, shadingN); // Note: reflect(..) is not given in the framework.
            double specAngle = std::max(reflectDir.dot(V), 0.0);
            double specular = std::pow(specAngle, material.n);

            color += specular * material.ks * light->color;
        }
    }

    if (depth > 0 and material.isTransparent) {
        // The object is transparent, and thus refracts and reflects light.
        Vector reflectDir = reflect(ray.D, shadingN);
        Ray reflectRay(hit + (epsilon * shadingN), reflectDir);

        double DdotN = ray.D.dot(N);
        double ni = 1.0;
        double nt = material.nt;
        int outside = 0;

        if (DdotN < 0) {
            DdotN = -DdotN;
            outside = 1;
        } else {
            shadingN = -shadingN;
            N = -N;
            std::swap(nt, ni);
        }

        double refRatio = ni / nt;
        double k = 1 - refRatio * refRatio * (1 - DdotN * DdotN);

        Vector refractDir;
        if (k < 0) {
            refractDir = Vector(0);
        } else {
            refractDir = refRatio * ray.D + (refRatio * DdotN - sqrt(k)) * N;
        }
        refractDir.normalize();

        Vector refractRayFrom;
        if (outside == 1) {
            refractRayFrom = hit - (epsilon * shadingN);
        } else {
            refractRayFrom = hit + (epsilon * shadingN);
        }

        //Schlick's approximation to determine the ratio between the two.
        double kr0 = ((ni - nt) / (ni + nt)) * ((ni - nt) / (ni + nt));
        double kr = kr0 + (1.0 - kr0) * pow(1.0 - DdotN, 5);

        Ray refractRay(refractRayFrom, refractDir);

        color += trace(reflectRay, depth - 1) * kr +
            trace(refractRay, depth - 1) * (1.0 - kr);
    } else if (depth > 0 and material.ks > 0.0) {
        // The object is not transparent, but opaque.
        Vector reflectDir = reflect(ray.D, shadingN);
        Ray reflectRay(hit + (epsilon * shadingN), reflectDir);
        color += material.ks * trace(reflectRay, depth - 1);
    }

    return color;
}

void Scene::render(Image &img) {
    unsigned w = img.width();
    unsigned h = img.height();
    double add = 1 / ((double) supersamplingFactor + 1);

    for (unsigned y = 0; y < h; ++y)
        for (unsigned x = 0; x < w; ++x) {
            Color col = Color(0, 0, 0);
            for (unsigned i = 0; i < supersamplingFactor; ++i) {
                for (unsigned j = 0; j < supersamplingFactor; ++j) {
                    Point pixel(x + add * (j + 1), h - 1 - y + add * (i + 1), 0);
                    Ray ray(eye, (pixel - eye).normalized());
                    col += trace(ray, recursionDepth);
                }
            }
            col = col / (supersamplingFactor * supersamplingFactor);
            col.clamp();
            img(x, y) = col;
        }
}

// --- Misc functions ----------------------------------------------------------

// Defaults
Scene::Scene()
    :
    objects(),
    lights(),
    eye(),
    renderShadows(false),
    recursionDepth(0),
    supersamplingFactor(1) {}

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

void Scene::setRenderShadows(bool shadows) {
    renderShadows = shadows;
}

void Scene::setRecursionDepth(unsigned depth) {
    recursionDepth = depth;
}

void Scene::setSuperSample(unsigned factor) {
    supersamplingFactor = factor;
}
