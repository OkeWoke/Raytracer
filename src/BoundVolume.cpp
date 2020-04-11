#include "BoundVolume.h"

static const Vector plane_normals[7]= {Vector(1,0,0), Vector(0,1,0), Vector(0,0,1), Vector(sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3), Vector(sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3)};


BoundVolume::BoundVolume()
{


}

BoundVolume* BoundVolume::compute_bound_volume()
{
    BoundVolume* bv = new BoundVolume();

    return bv;
}

double BoundVolume::intersect(const Vector& src, const Vector& d)
{
    return -2;
}

Vector BoundVolume::normal(const Vector& p)
{
    return Vector(0,0,0);
}
