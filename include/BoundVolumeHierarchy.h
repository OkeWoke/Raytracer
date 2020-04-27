#include "GObjects/Triangle.h"
#include "BoundVolume.h"
#include "GObjects/GObject.h"
#include<vector>

#ifndef BOUNDVOLUMEHIERARCHY_H
#define BOUNDVOLUMEHIERARCHY_H


class BoundVolumeHierarchy
{
    public:
        BoundVolumeHierarchy();
        BoundVolumeHierarchy(BoundVolume* bv,  Vector center);
        BoundVolumeHierarchy(Vector& diameter, Vector& center);
        virtual ~BoundVolumeHierarchy();

        BoundVolumeHierarchy* children[8];
        std::vector<Triangle*> triangles;

        BoundVolume* bv = nullptr;

        void insert_triangle(Triangle* tri, int depth);
        BoundVolume* build_BVH();
        GObject::intersection intersect(const Vector& src, const Vector& d);

        Vector center;
        Vector diameter;


    protected:

    private:
        int MAX_DEPTH= 16;
        bool is_leaf = true;
};

#endif // BOUNDVOLUMEHIERARCHY_H
