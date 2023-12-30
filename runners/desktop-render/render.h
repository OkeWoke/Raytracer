#pragma once
#include "GObject.h"
#include "Vector.h"
#include "Color.h"
#include "BoundVolumeHierarchy.h"
#include "Sampler.h"
#include "Camera.h"
#include "imageArray.h"
#include "Light.h"

struct Hit
{
    Vector src;
    Vector ray_dir;
    Vector n;
    double t;
    GObject* obj;
    Color color;
};

struct Config
{
    int threads_to_use;
    int max_reflections;
    int spp;
    std::string stretch;
};

struct Scene
{
    std::vector<std::shared_ptr<GObject>> objects;
    std::vector<Light> lights;
    std::vector<std::shared_ptr<GObject>> gLights; //GObjects that have emission.
    Camera cam;
    Config config;
};

bool is_light(GObject* obj);
Hit intersect(const Vector& src, const Vector& ray_dir, const BoundVolumeHierarchy& bvh);
void cast_rays_multithread(const Scene& scene,
                           ImageArray& img,
                           const Sampler& sampler1,
                           const Sampler& sampler2,
                           const BoundVolumeHierarchy& bvh
                           );

Color trace_rays_iterative(const Vector& origin,
                           const Vector& ray_dir,
                           const BoundVolumeHierarchy& bvh,
                           const Config& config,
                           int depth,
                           const Sampler& ha1,
                           const Sampler& ha2,
                           const std::vector<std::shared_ptr<GObject>>& objects,
                           const std::vector<std::shared_ptr<GObject>>& gLights);

Color shade(const Hit& hit,
            int reflection_count,
            const Sampler& ha1,
            const Sampler& ha2,
            const BoundVolumeHierarchy& bvh,
            const Config& config,
            const std::vector<std::shared_ptr<GObject>>& objects,
            const std::vector<Light>& lights);