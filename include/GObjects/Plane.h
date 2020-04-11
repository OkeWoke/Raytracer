#ifndef PLANE_H
#define PLANE_H
#include "GObjects/GObject.h"
#include <String>
#include "ext/Markup.h"
class Plane: public GObject
{
    public:
        Plane();
        Plane(Vector pos, Vector n, double l, double w, Color c, double shininess, double reflectivity);
        Vector normal(const Vector& p);
        double intersect(const Vector& src, const Vector& d);
        void deserialize(std::string strSubDoc);

    private:
        Vector n;
        double l;
        double w;
};

#endif // PLANE_H
