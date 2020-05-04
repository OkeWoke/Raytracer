#include "BoundVolume.h"
#include "Matrix.h"
#ifndef PLANE_H
#define PLANE_H
#include "GObjects/GObject.h"
#include <String>
#include "ext/Markup.h"

class Plane: public GObject
{
    public:
        Plane();
        ~Plane();
        Plane(Vector pos, Vector n, double l, double w, Color c, double shininess, double reflectivity);
        intersection intersect(const Vector& src, const Vector& d);
        void deserialize(std::string strSubDoc);
        Vector n;
        Vector u;
        Vector v;
        double l;
        double w;

    private:

};

#endif // PLANE_H
