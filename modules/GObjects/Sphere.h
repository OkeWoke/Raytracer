#pragma once

#include <string>
#include <iostream>

#include "GObject.h"
#include "Markup.h"
#include "BoundVolume.h"

class Sphere: public GObject
{
    public:
        Sphere();
        ~Sphere();
        Sphere(Vector c, double radius);
        Sphere(Vector pos, double radius, Color c, double shininess, double reflectivity);
        intersection intersect(const Vector& src, const Vector& d);

        void deserialize(std::string strSubDoc);
        double radius;
        Vector get_random_point(double val1, double val2);
    //private:

};
