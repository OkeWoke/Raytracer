#pragma once

#include <vector>
#include <queue>
#include <functional>

#include "Triangle.h"
#include "BoundVolume.h"
#include "GObject.h"
#include "Sphere.h"

class BoundVolumeHierarchy
{
    public:
        BoundVolumeHierarchy() {};
        BoundVolumeHierarchy(std::vector<std::shared_ptr<GObject>>& objects);
        BoundVolumeHierarchy(std::vector<Vector>& vertices);

        BoundVolumeHierarchy(Vector& diameter, Vector& center);
        ~BoundVolumeHierarchy() {};

        std::unique_ptr<BoundVolumeHierarchy> children[8];
        std::vector<GObject*> objects = {};

        std::shared_ptr<BoundVolume> bv;

        void insert_object(GObject* tri, int depth);
        std::shared_ptr<BoundVolume>  build_BVH();
        GObject::intersection intersect(const Vector& src, const Vector& d, int depth) const;
        GObject::intersection bv_intersect(const Vector& src, const Vector& d);
        GObject::intersection priority_intersect(const Vector& src, const Vector& d, int depth);
        Vector center;
        Vector diameter;

    private:
        void BoundSetup(std::shared_ptr<BoundVolume> bv, Vector& center);
        int MAX_DEPTH= 15;
        bool is_leaf = true;
};
