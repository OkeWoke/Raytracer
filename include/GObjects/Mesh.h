#include "GObjects/GObject.h"
#include "GObjects/Triangle.h"
#include <vector>
#include<limits>
#include <sstream>
#include<fstream>
#include "Matrix.h"
#include "Sphere.h"
#include "Utility.h"

#ifndef MESH_H
#define MESH_H

class Mesh : public GObject
{
    public:
        Mesh();
        ~Mesh();
        Mesh(std::string filename);
        //Mesh(std::vector<Triangle*> triangles, Vector center);

        intersection intersect(const Vector& src, const  Vector& d);
        Vector normal(const Vector& p);
        Vector center;

        std::vector<Triangle*> triangles;

        Sphere bounding_sphere;
        void obj_reader(std::string filename);
        void deserialize(std::string strSubDoc);

        Mesh* get_obj();
    private:
        Matrix trans_mat;
        GObject* tri;

};

#endif // MESH_H
