#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Light.h"
#include "GObject.h"
#include "Camera.h"

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