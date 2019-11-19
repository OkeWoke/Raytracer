#include "GObjects/Mesh.h"

Mesh::Mesh()
{
    //ctor
}

Mesh::Mesh(std::vector<Triangle*> triangles, Vector c):
triangles(triangles),
center(c)
{
//stuff
}

double Mesh::intersect(Vector src, Vector d)
{
    return -1;
}

Vector Mesh::normal(Vector p)
{
    return Vector(0,0,0);
}
