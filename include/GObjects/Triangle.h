#include "GObjects/GObject.h"
#include <sstream>
#include <string>

#ifndef TRIANGLE_H
#define TRIANGLE_H


class Triangle: public GObject
{
    public:
        Triangle();
        Triangle(Vector p1, Vector p2, Vector p3, Vector vt1, Vector vt2, Vector vt3, Vector vn1, Vector vn2, Vector vn3);
        intersection intersect(const Vector& src, const Vector& d);
        Vector normal(const Vector& p);
        std::string to_string();
        Vector v1, v2, v3;

        Triangle* get_obj();
        void deserialize(std::string strSubDoc);
    private:
        Vector vt1, vt2, vt3; //texture coords only use x,y, z=0
        Vector vn1, vn2, vn3;
        Vector n;
};

#endif // TRIANGLE_H
