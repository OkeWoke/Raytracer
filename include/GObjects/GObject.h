#ifndef GOBJECT_H
#define GOBJECT_H

#include "Vector.h"
#include "Color.h"

class GObject
{
    public:
        GObject();
        Color ambient;
        Color diffuse;
        Color specular;
        Vector position;
        double shininess;
        double reflectivity;

        virtual double intersect(Vector src, Vector d) = 0;
        virtual Vector normal(Vector p) = 0;
        //virtual std::string Serialize();
        //virtual void Deserialize(std::string strSubDoc);
};

#endif // GOBJECT_H
