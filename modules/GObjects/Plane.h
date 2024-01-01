#pragma once

#include <string>

#include "Matrix.h"
#include "GObject.h"


class Plane: public GObject
{
    public:
        Plane();
        ~Plane();
        Plane(Vector pos,
              Vector n,
              double l,
              double w,
              Color c,
              double shininess,
              double reflectivity,
              int brdf,
              Color emission);
        Plane(const Plane& p);
        intersection intersect(const Vector& src, const Vector& d);
        Vector get_random_point(double val1, double val2);
        Vector n;
        Vector u;
        Vector v;
        double l;
        double w;

    private:

};

