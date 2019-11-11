#ifndef SPHERE_H
#define SPHERE_H
#include "GObject.h"
#include "Markup.h"
#include <string>
#include <iostream>
class Sphere: public GObject
{
    public:
        Sphere();
        Sphere(Vector pos, double radius, Color a, Color d, Color s, double shininess, double reflectivity);
        double intersect(Vector src, Vector d);
        Vector normal(Vector p);

        void deserialize(std::string strSubDoc);
    private:
        double radius;
};

#endif // SPHERE_H
