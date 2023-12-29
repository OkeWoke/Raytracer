#pragma once

#include <vector>
#include <limits>
#include <sstream>
#include <fstream>

#include "pngpp-w.hpp"
#include "Matrix.h"
#include "Sphere.h"
#include "Utility.h"
#include "GObject.h"
#include "Triangle.h"
#include "BoundVolumeHierarchy.h"
#include "BoundVolume.h"

struct MeshConfig
{
    std::string filename;
    double shininess;
    double reflectivity;
    double brdf;
    Vector position;
    Matrix mat;
    Color color;
    std::string texture_filename;
};

class Mesh : public GObject
{
    public:
        Mesh(std::vector<Vector>& vertices, std::vector<std::shared_ptr<Triangle>>& triangles, const MeshConfig& config);
        ~Mesh(){};

        intersection intersect(const Vector& src, const  Vector& d);
        Vector center;

        std::vector<std::shared_ptr<Triangle>> triangles;
        std::vector<Vector> vertices;

        BoundVolumeHierarchy bvh;
        Vector get_random_point(double val1, double val2);
    private:
        Matrix mat;
        GObject* tri;
        png::image< png::rgb_pixel> texture;
};
