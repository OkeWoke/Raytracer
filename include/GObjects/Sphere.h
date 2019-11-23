#ifndef SPHERE_H
#define SPHERE_H
#include "GObjects/GObject.h"
#include "ext/Markup.h"
#include <string>
#include <iostream>
class Sphere: public GObject
{
    public:
        Sphere();
        Sphere(Vector c, double radius);
        Sphere(Vector pos, double radius, Color c, double shininess, double reflectivity);
        double intersect(const Vector& src, const Vector& d);
        Vector normal(const Vector& p);

        void deserialize(std::string strSubDoc);

    private:
        double radius;
};

#endif // SPHERE_H
