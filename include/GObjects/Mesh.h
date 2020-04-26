#include <vector>
#include<limits>
#include <sstream>
#include<fstream>
#include <png.hpp>

#include "Matrix.h"
#include "Sphere.h"
#include "Utility.h"

#include "GObjects/GObject.h"
#include "GObjects/Triangle.h"
#include "BoundVolumeHierarchy.h"

#ifndef MESH_H
#define MESH_H

#include "BoundVolume.h"
class Mesh : public GObject
{
    public:
        Mesh();
        ~Mesh();
        Mesh(std::string filename);

        intersection intersect(const Vector& src, const  Vector& d);
        Vector center;

        std::vector<Triangle*> triangles;
        std::vector<Vector> vertices;

        void obj_reader(std::string filename);
        void deserialize(std::string strSubDoc);

        BoundVolume* bv;
        BoundVolumeHierarchy* bvh;

    private:
        Matrix mat;
        GObject* tri;
        png::image< png::rgb_pixel> texture;

};

#endif // MESH_H
