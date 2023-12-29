#pragma once

#include <cmath>
#include <vector>
#include <Vector.h>
#include <limits>

#include "GObject.h"
#include "Sphere.h"
#include "Plane.h"

class BoundVolume : public GObject//accelerated structure unit. Contains 7 plane normals, computes 14 t values for intersections.
{
    public:
        BoundVolume(){};
        ~BoundVolume(){};

        intersection intersect(const Vector& src, const Vector& d);

        double d_min_vals[7];
        double d_max_vals[7];

        Vector center;
        static const Vector plane_normals[7];
        static std::shared_ptr<BoundVolume> compute_bound_volume(std::vector<Vector>& vertices);
        static std::shared_ptr<BoundVolume> compute_bound_volume(std::vector<std::shared_ptr<BoundVolume>>& volumes);
        static std::shared_ptr<BoundVolume> compute_bound_volume(std::vector<std::shared_ptr<GObject>>& objects);

        static std::shared_ptr<BoundVolume>  compute_bound_volume(Sphere* sphere);
        static std::shared_ptr<BoundVolume> compute_bound_volume(Plane* plane);
        Vector get_random_point(double val1, double val2);
};
