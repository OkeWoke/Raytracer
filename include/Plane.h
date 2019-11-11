#ifndef PLANE_H
#define PLANE_H
#include "GObject.h"
#include <String>
#include "Markup.h"
class Plane: public GObject
{
    public:
        Plane();
        Plane(Vector pos, Vector n, double l, double w, Color a, Color d, Color s, double shininess, double reflectivity);
        Vector normal(Vector p);
        double intersect(Vector src, Vector d);
        void deserialize(std::string strSubDoc);

    private:
        Vector n;
        double l;
        double w;
};

#endif // PLANE_H
