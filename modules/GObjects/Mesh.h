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



class Mesh : public GObject
{
    public:
        Mesh();
        ~Mesh();

        intersection intersect(const Vector& src, const  Vector& d);
        Vector center;

        std::vector<Triangle*> triangles;
        std::vector<Vector> vertices;

        void obj_reader(std::string filename);
        void deserialize(std::string strSubDoc);

        //BoundVolume* bv;
        BoundVolumeHierarchy* bvh;
        Vector get_random_point(double val1, double val2);
    private:
        Matrix mat;
        GObject* tri;
        png::image< png::rgb_pixel> texture;
};
