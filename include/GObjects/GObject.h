

#ifndef GOBJECT_H
#define GOBJECT_H

#include "Vector.h"
#include "Color.h"

class GObject
{



    public:
        Color color;
        Vector position;
        double shininess;
        double reflectivity;

        GObject();
        GObject(Vector position): position(position){};
        GObject(Color c, Vector pos, double shininess, double reflectivity):color(c), position(pos), shininess(shininess), reflectivity(reflectivity){};

        struct intersection
        {
            GObject* obj_ref;
            double t;
            Vector n;
            Color color;
            intersection():obj_ref(nullptr), t(-1),color(Color(-1,-1,-1))
            {
            };

            ~intersection()
            {
                obj_ref = nullptr;
            }
        };

        virtual intersection intersect(const Vector& src, const Vector& d) = 0;
};

#endif // GOBJECT_H
