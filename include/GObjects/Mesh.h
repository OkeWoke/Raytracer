#ifndef MESH_H
#define MESH_H

#include "GObjects/GObject.h"
#include "GObjects/Triangle.h"
#include <vector>
#include<limits>
#include "Matrix.h"
#include "Sphere.h"

class Mesh : public GObject
{
    public:
        Mesh();
        ~Mesh();
        Mesh(std::vector<Triangle*> triangles, Vector center);

        double intersect(const Vector& src, const  Vector& d);
        Vector normal(const Vector& p);
        Vector center;

        std::vector<Triangle*> triangles;

        Sphere bounding_sphere;

    private:
        Matrix trans_mat;
        GObject* tri;

};

#endif // MESH_H
