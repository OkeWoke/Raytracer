#ifndef SPHERE_H
#define SPHERE_H
#include "GObject.h"

class Sphere: public GObject
{
    public:
        Sphere();
        Sphere(Vector pos, double radius, Color a, Color d, Color s, double shininess);
        double intersect(Vector src, Vector d);
        Vector normal(Vector p);
    private:
        double radius;
};

#endif // SPHERE_H
