#include "GObjects/Mesh.h"
#include<iostream>
Mesh::Mesh()
{
    //ctor
}

Mesh::Mesh(std::vector<Triangle*> triangles, Vector c):
triangles(triangles),
center(c)
{
    trans_mat = Matrix::translate(center);
    this->bounding_sphere = Sphere(center, 5);
}

Mesh::~Mesh()
{
    for (auto p : triangles)
    {
     delete p;
    }
    triangles.clear();
}

double Mesh::intersect(const Vector& src, const  Vector& d)
{
    if(bounding_sphere.intersect(src, d) != -1)
    {
        double t = -1;
        double closest_t = std::numeric_limits<double>::max();
        for(unsigned int i = 0; i < triangles.size(); i++)
        {
            double tmp = triangles[i]->intersect(src, d);
            if(tmp > 0 && tmp < closest_t)
            {
                closest_t = tmp;
                tri = triangles[i];
                //std::cout << "Mesh hit, trignale found " << tmp << std::endl;
            } //finds closest triangle to intersect
        }
        if (closest_t != std::numeric_limits<double>::max())
        {
            return closest_t;
        }
    }

    return -1;
}

Vector Mesh::normal(const Vector& p)
{
    return tri->normal(p);
}
