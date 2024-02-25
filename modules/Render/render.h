#pragma once
#include "GObject.h"
#include "Vector.h"
#include "Color.h"
#include "BoundVolumeHierarchy.h"
#include "Sampler.h"
#include "Camera.h"
#include "imageArray.h"
#include "Light.h"
#include "RenderStructs.h"

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
