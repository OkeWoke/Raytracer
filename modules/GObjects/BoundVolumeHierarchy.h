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
    private:
        void BoundSetup(std::shared_ptr<BoundVolume> bv, Vector& center);
        int MAX_DEPTH= 15;
        bool is_leaf = true;
        const static int NUM_CHILDREN = 8;
    public:
        BoundVolumeHierarchy() {};
        BoundVolumeHierarchy(std::vector<std::shared_ptr<GObject>>& objects);
        BoundVolumeHierarchy(std::vector<Vector>& vertices);

        BoundVolumeHierarchy(Vector& diameter, Vector& center);
        ~BoundVolumeHierarchy() {};

        std::unique_ptr<BoundVolumeHierarchy> children[NUM_CHILDREN];
        std::vector<std::shared_ptr<GObject>> objects = {};

        std::shared_ptr<BoundVolume> bv;

        void insert_object(std::shared_ptr<GObject> tri, int depth);
        std::shared_ptr<BoundVolume>  build_BVH();
        GObject::intersection intersect(const Vector& src, const Vector& d, int depth) const;
        Vector center;
        Vector diameter;
};
