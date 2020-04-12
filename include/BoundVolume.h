#ifndef BOUNDVOLUME_H
#define BOUNDVOLUME_H

#include <VObject.h>
#include<cmath>
#include<vector>
class BoundVolume : public VObject
//accelerated structure unit. Contains 7 plane normals, computes 14 t values for intersections.
{
    public:
        BoundVolume();
        BoundVolume* compute_bound_volume();
        double intersect(const Vector& src, const Vector& d);
        Vector normal(const Vector& p);

    private:
        static const Vector plane_normals[7];


        std::vector<Vector*> vertices;
        double d_min[7];
        double d_max[7];


};

#endif // BOUNDVOLUME_H
