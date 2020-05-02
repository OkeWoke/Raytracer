#include "GObjects/Triangle.h"
#include "BoundVolume.h"
#include "GObjects/GObject.h"
#include "GObjects/Sphere.h"
#include<vector>

#ifndef BOUNDVOLUMEHIERARCHY_H
#define BOUNDVOLUMEHIERARCHY_H


class BoundVolumeHierarchy
{
    public:
        BoundVolumeHierarchy();
        BoundVolumeHierarchy(GObject* bv,  Vector center);
        BoundVolumeHierarchy(Vector& diameter, Vector& center);
        virtual ~BoundVolumeHierarchy();

        BoundVolumeHierarchy* children[8];
        std::vector<GObject*> objects;

        BoundVolume* bv = nullptr;

        void insert_object(GObject* tri, int depth);
        BoundVolume* build_BVH();
        GObject::intersection intersect(const Vector& src, const Vector& d, int depth);

        Vector center;
        Vector diameter;


    protected:

    private:
        int MAX_DEPTH= 15;
        bool is_leaf = true;
};

#endif // BOUNDVOLUMEHIERARCHY_H
