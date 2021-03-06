


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
        Color emission;
        int brdf;
        GObject();
        virtual ~GObject();
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
                //obj_ref = nullptr;
            }
        };

        virtual intersection intersect(const Vector& src, const Vector& d) = 0;
        virtual Vector get_random_point(double val1, double val2) = 0;
        GObject* bv = nullptr; //we will need to forcefully cast this to BoundVolume type in each usecase...
};

#endif // GOBJECT_H
