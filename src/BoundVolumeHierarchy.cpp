#include "BoundVolumeHierarchy.h"

BoundVolumeHierarchy::BoundVolumeHierarchy()
{
    //ctor
}

BoundVolumeHierarchy::~BoundVolumeHierarchy()
{
    //dtor
}


void BoundVolumeHierarchy::insert_triangle(Triangle& tri)
{

}

BoundVolume* BoundVolumeHierarchy::build_BVH()
{
    vector<BoundVolume*> child_volumes;
    for (int i=0;i<8;i++)
    {
        if (children[i]!= nullptr)
        {
            BoundVolume* bv = children[i]->build_BVH();
            if (bv!= nullptr)
            //if child contains something...
            {
                child_volumes.push_back(bv);
            }
        }
    }

    if (child_volumes.size() == 0)
    //No child nodes... this is a leaf, possibly containing triangles.
    {
        if(triangles.size() == 0)
        //leaf with no triangles? fuckin useless node aren't we? Return no bv.
        {
            this->bv = nullptr;
        }else
        {
            vector<Vector> vertices;
            for (int i =0; i<triangles.size();i++)
            {
                vertices.push_back(triangles[i]->v1);
                vertices.push_back(triangles[i]->v2);
                vertices.push_back(triangles[i]->v3);
            }
            this->bv = BoundVolume::compute_bound_volume(vertices);
            //at this point we could clear memory of triangles as the instance doesn't need it anymore..., although
        }

    }else
    //We have child bounded volumes, create bvh around this
    {
        this->bv = BoundVolume::compute_bound_volume(child_volumes);
    }

    return this->bv;
}

intersection BoundVolumeHierarchy::intersect()
{

}
