#ifndef MESH_H
#define MESH_H

#include "GObjects/GObject.h"
#include "GObjects/Triangle.h"
#include <vector>
class Mesh : public GObject
{
    public:
        Mesh();
        Mesh(std::vector<Triangle*> triangles, Vector center);

        double intersect(Vector src, Vector d);
        Vector normal(Vector p);
        Vector center;
        std::vector<Triangle*> triangles;

    protected:

    private:
};

#endif // MESH_H
