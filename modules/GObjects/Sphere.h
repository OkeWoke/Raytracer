#pragma once

#include <string>
#include <iostream>

#include "GObject.h"


class Sphere: public GObject
{
    public:
        Sphere();
        ~Sphere();
        Sphere(Vector c, double radius);
        Sphere(Vector pos, double radius, Color c, double shininess, double reflectivity);
        Sphere(const Sphere& s);
        intersection intersect(const Vector& src, const Vector& d);

        double radius;
        Vector get_random_point(double val1, double val2);
};
