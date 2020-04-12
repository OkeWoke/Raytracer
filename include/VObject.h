#ifndef VOBJECT_H
#define VOBJECT_H
#include "Vector.h"

class VObject //volume object
{
    public:
        VObject();
        virtual double intersect(const Vector& src, const Vector& d) = 0;
        virtual Vector normal(const Vector& p) = 0;
};

#endif // VOBJECT_H
