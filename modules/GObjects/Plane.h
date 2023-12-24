#pragma once

#include <String>

#include "BoundVolume.h"
#include "Matrix.h"
#include "GObject.h"
#include "Markup-w.h"


class Plane: public GObject
{
    public:
        Plane();
        ~Plane();
        Plane(Vector pos, Vector n, double l, double w, Color c, double shininess, double reflectivity);
        intersection intersect(const Vector& src, const Vector& d);
        void deserialize(std::string strSubDoc);
        Vector get_random_point(double val1, double val2);
        Vector n;
        Vector u;
        Vector v;
        double l;
        double w;

    private:

};

