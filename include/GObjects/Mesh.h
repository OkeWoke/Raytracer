#include <vector>
#include<limits>
#include <sstream>
#include<fstream>
#include "Matrix.h"
#include "Sphere.h"
#include "Utility.h"

#include "GObjects/GObject.h"
#include "GObjects/Triangle.h"


#ifndef MESH_H
#define MESH_H

#include "BoundVolume.h"
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
        std::vector<Vector> vertices;
        Sphere bounding_sphere;
        void obj_reader(std::string filename);
        void deserialize(std::string strSubDoc);

        Mesh* get_obj();

        BoundVolume* bv;

    private:
        Matrix trans_mat;
        GObject* tri;

};

#endif // MESH_H
