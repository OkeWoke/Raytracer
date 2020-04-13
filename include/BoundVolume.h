#include<cmath>
#include<vector>
#include<Vector.h>
#include<limits>
#include<GObjects/GObject.h>
#include<GObjects/Mesh.h>
#ifndef BOUNDVOLUME_H
#define BOUNDVOLUME_H


class BoundVolume//accelerated structure unit. Contains 7 plane normals, computes 14 t values for intersections.
{
    public:
        BoundVolume();


        static BoundVolume* compute_bound_volume(std::vector<GObject*> &objects);

        double intersect(const Vector& src, const Vector& d);
        Vector normal(const Vector& p);
        double d_min_vals[7];
        double d_max_vals[7];
    private:
        static const Vector plane_normals[7];


        std::vector<Vector*> vertices;



};

#endif // BOUNDVOLUME_H
