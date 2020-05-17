#include "GObjects/GObject.h"
#include <sstream>
#include <string>
#include <png.hpp>
#include <math.h>
#include "Utility.h"
#ifndef TRIANGLE_H
#define TRIANGLE_H


class Triangle: public GObject
{
    public:
        Triangle();
        Triangle(const Vector& p1, const Vector& p2, const Vector& p3, const Vector& vt1, const Vector& vt2, const Vector& vt3, const Vector& vn1, const Vector& vn2, const Vector& vn3);
        intersection intersect(const Vector& src, const Vector& d, const png::image< png::rgb_pixel >& texture);
        intersection intersect(const Vector& src, const Vector& d);
        std::string to_string();
        Vector v1, v2, v3;
        void deserialize(std::string strSubDoc);
        Vector get_random_point(double val1, double val2);

    private:
        Vector vt1, vt2, vt3; //texture coords only use x,y, z=0
        Vector vn1, vn2, vn3;
        Vector n;

        Vector AB, AC;
        double area;

};

#endif // TRIANGLE_H
