#include<cmath>
#include<vector>
#include<Vector.h>
#include<limits>
#include"GObjects/GObject.h"

#ifndef BOUNDVOLUME_H
#define BOUNDVOLUME_H
#include "GObjects/Sphere.h"
#include "GObjects/Plane.h"
class BoundVolume : public GObject//accelerated structure unit. Contains 7 plane normals, computes 14 t values for intersections.
{
    public:
        BoundVolume();
        ~BoundVolume();

        intersection intersect(const Vector& src, const Vector& d);

        double d_min_vals[7];
        double d_max_vals[7];

        Vector center;
        static const Vector plane_normals[7];
        static BoundVolume* compute_bound_volume(std::vector<Vector>& vertices);
        static BoundVolume* compute_bound_volume(std::vector<BoundVolume*> volumes);
        static BoundVolume* compute_bound_volume(std::vector<GObject*> objects);

        static BoundVolume* compute_bound_volume(Sphere* sphere);
        static BoundVolume* compute_bound_volume(Plane* plane);
        Vector get_random_point(double val1, double val2);
};


#endif // BOUNDVOLUME_H
