#include "BoundVolumeHierarchy.h"
BoundVolumeHierarchy::BoundVolumeHierarchy( )
{
    //ctor
}
BoundVolumeHierarchy::BoundVolumeHierarchy(BoundVolume* bv, Vector center)
{
    this->bv = bv;
    //std::cout << center.to_string() << std::endl;
    this->center = center;
    if(bv != nullptr)
    //this is the root node!
    {
        //compute the center of this node from the bv, and the width!
        //set these values into Vectors diameter and center.
        //center.x = (bv->d_max_vals[0] + bv->d_min_vals[0])/2;
        //center.y = (bv->d_max_vals[1] + bv->d_min_vals[1])/2;
        //center.z = (bv->d_max_vals[1] + bv->d_min_vals[2])/2;

        diameter.x = abs(bv->d_max_vals[0] - bv->d_min_vals[0]);
        diameter.y = abs(bv->d_max_vals[1] - bv->d_min_vals[1]);
        diameter.z = abs(bv->d_max_vals[2] - bv->d_min_vals[2]);
        std::cout<<diameter.to_string() <<std::endl;
        std::cout<<center.to_string() << std::endl;
    }

    for (int i=0;i<8;i++)
    {
        this->children[i] = nullptr;
    }
}

BoundVolumeHierarchy::BoundVolumeHierarchy(Vector& diameter, Vector& center)
{
    this->center=center;
    this->diameter = diameter;
    for (int i=0;i<8;i++)
    {
        this->children[i] = nullptr;
    }
}

BoundVolumeHierarchy::~BoundVolumeHierarchy()
{
    for (auto p : children)
    {
        delete p;
    }
    delete bv;
}


void BoundVolumeHierarchy::insert_triangle(Triangle* tri, int depth)
{
    //base case
    if (depth >= MAX_DEPTH || (triangles.size() == 0 && is_leaf))
    {
        triangles.push_back(tri);
        return;
    }

    is_leaf = false;

    if (triangles.size()==1 )
    {
        auto tmp_tri = triangles[0];
        triangles.clear();
        this->insert_triangle(tmp_tri, depth);
    }
    int key = 0;


    if (tri->position.x > this->center.x)
    {
        key++;
    }
    if (tri->position.y < this->center.y)
    {
        key = key+2;
    }
    if(tri->position.z > this->center.z)
    {
        key= key+4;
    }

    if(children[key] == nullptr)
    {
        Vector diameter = this->diameter/2;
        Vector center;

        if (key >=4)
        {
            center.z = this->center.z + diameter.z/2;
        }else
        {
            center.z = this->center.z - diameter.z/2;
        }

        if (key == 0 || key == 1 || key == 4 || key == 5)
        {
            center.y = this->center.y + diameter.y/2;
        }else
        {
            center.y = this->center.y - diameter.y/2;
        }

        if(key%2 == 1)
        {
            center.x = this->center.x + diameter.x/2;
        }else
        {
            center.x = this->center.x - diameter.x/2;
        }

        children[key] = new BoundVolumeHierarchy(diameter, center);
    }

    children[key]->insert_triangle(tri, depth+1);
}

BoundVolume* BoundVolumeHierarchy::build_BVH()
{
    std::vector<BoundVolume*> child_volumes;
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
            std::vector<Vector> vertices;
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

GObject::intersection BoundVolumeHierarchy::intersect(const Vector& src, const Vector& d)
//potential speed up is by making another intersect function that just returns bool instead of intersection obj.
{
    GObject::intersection bv_inter = GObject::intersection();
    if(this->bv == nullptr)
        //this case shouldnt happen?
    {
        return bv_inter;
    }

    bv_inter = bv->intersect(src,d);
    GObject::intersection best_inter = GObject::intersection();
    if(bv_inter.obj_ref != nullptr)
    //we have an intersection...
    {
        double min_t = std::numeric_limits<double>::max();

        if(is_leaf)
        {
            for(int i=0; i<triangles.size();i++)
            {
                //std::cout << triangles[i] << std::endl;
                GObject::intersection tmp = triangles[i]->intersect(src, d);
                if(tmp.obj_ref != nullptr)
                //there was a triangle intersection...
                {
                    if(tmp.t<min_t)
                    {
                        min_t = tmp.t;
                        best_inter = tmp;
                    }
                }

            }

        }else
        {
            for (int i=0;i<8;i++)
            {
                if (children[i] != nullptr)
                {
                    GObject::intersection tmp = children[i]->intersect(src, d);
                    if(tmp.obj_ref != nullptr)
                    {
                        if (tmp.t < min_t)
                        {
                            min_t = tmp.t;
                            best_inter = tmp;
                        }
                    }
                }
            }
        }
    }

    return best_inter;
}
