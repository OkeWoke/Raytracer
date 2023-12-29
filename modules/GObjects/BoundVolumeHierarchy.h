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
        BoundVolumeHierarchy();
        BoundVolumeHierarchy(std::vector<GObject*>& objects);
        BoundVolumeHierarchy(std::vector<Vector>& vertices);

        BoundVolumeHierarchy(Vector& diameter, Vector& center);
        virtual ~BoundVolumeHierarchy();

        BoundVolumeHierarchy* children[8];
        std::vector<GObject*> objects = {};

        BoundVolume* bv = nullptr;

        void insert_object(GObject* tri, int depth);
        BoundVolume* build_BVH();
        GObject::intersection intersect(const Vector& src, const Vector& d, int depth) const;
        GObject::intersection bv_intersect(const Vector& src, const Vector& d);
        GObject::intersection priority_intersect(const Vector& src, const Vector& d, int depth);
        Vector center;
        Vector diameter;

    private:
        void BoundSetup(BoundVolume* bv, Vector& center);
        int MAX_DEPTH= 15;
        bool is_leaf = true;
};
