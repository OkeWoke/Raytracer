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


    ~Hit()
    {
        obj = nullptr;
    }
};

struct Config
{
    int threads_to_use;
    int max_reflections;
    int spp;
    std::string stretch;
};

bool is_light(GObject* obj);
Hit intersect(const Vector& src, const Vector& ray_dir, BoundVolumeHierarchy* bvh);
void cast_rays_multithread(const Config& config, const Camera& cam, const ImageArray& img, Sampler* sampler1, Sampler* sampler2, BoundVolumeHierarchy* bvh, const std::vector<GObject*>& objects, const std::vector<Light>& lights, const std::vector<GObject*>& gLights);
Color trace_rays_iterative(const Vector& origin, const Vector& ray_dir, BoundVolumeHierarchy* bvh, const Config& config, int depth, Sampler* ha1, Sampler* ha2, const std::vector<GObject*>& objects, const std::vector<GObject*>& gLights);
Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2, BoundVolumeHierarchy* bvh, const Config& config, const std::vector<GObject*>& objects, const std::vector<Light>& lights);
