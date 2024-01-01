#pragma once

#include <sstream>
#include <string>
#include <math.h>

#include "pngpp-w.hpp"
#include "GObject.h"
#include "Utility.h"

class Triangle: public GObject
{
    public:
        Triangle();
        Triangle(Vector v[3], Vector vt[3], Vector vn[3]);
        intersection intersect(const Vector& src, const Vector& d, const png::image< png::rgb_pixel >& texture);
        intersection intersect(const Vector& src, const Vector& d);
        std::string to_string();
        Vector v[3];
        Vector get_random_point(double val1, double val2);

    private:
        Vector vt[3]; //texture coords only use x,y, z=0
        Vector vn[3];
        Vector n;

        Vector AB, AC;
        double area;
};
