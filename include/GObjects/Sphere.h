#include "GObjects/GObject.h"
#include "ext/Markup.h"
#include <string>
#include <iostream>

#ifndef SPHERE_H
#define SPHERE_H

class Sphere: public GObject
{
    public:
        Sphere();
        Sphere(Vector c, double radius);
        Sphere(Vector pos, double radius, Color c, double shininess, double reflectivity);
        intersection intersect(const Vector& src, const Vector& d);

        void deserialize(std::string strSubDoc);

    private:
        double radius;
};

#endif // SPHERE_H