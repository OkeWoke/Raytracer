#include <Triangle.h>
#include "BoundVolume.h"

#ifndef BOUNDVOLUMEHIERARCHY_H
#define BOUNDVOLUMEHIERARCHY_H


class BoundVolumeHierarchy
{
    public:
        BoundVolumeHierarchy();
        virtual ~BoundVolumeHierarchy();

        BoundVolumeHierarchy* children[8];
        vector<Triangle*> triangles;

        BoundVolume* bv;

        void insert_triangle(Triangle& tri);
        BoundVolume* build_BVH();
        intersection intersect();


    protected:

    private:
};

#endif // BOUNDVOLUMEHIERARCHY_H
