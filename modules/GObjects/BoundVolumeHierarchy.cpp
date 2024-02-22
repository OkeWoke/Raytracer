#include "BoundVolumeHierarchy.h"

BoundVolumeHierarchy::BoundVolumeHierarchy(std::vector<std::shared_ptr<GObject>>& objects)
{
    std::shared_ptr<BoundVolume> scene_bv = BoundVolume::compute_bound_volume(objects);
    Vector center = Vector(0,0,0);
    for(unsigned int k = 0; k < objects.size(); k++)
    {
        center  = center + objects[k]->position;
    }
    center = center / objects.size();

    BoundSetup(scene_bv, center);

    for (auto obj: objects)
    {
        this->insert_object(obj,0);
    }
    (void) this->build_BVH();
}

BoundVolumeHierarchy::BoundVolumeHierarchy(std::vector<Vector>& vertices)
{
    auto bv = BoundVolume::compute_bound_volume(vertices); //this is deleted by bvh destructor?
    Vector center = Vector(0,0,0);
    for(unsigned int k = 0; k < vertices.size(); k++)
    {
        center  = center + vertices[k];
    }
    center = center / vertices.size();

    BoundSetup(bv, center);
}

void BoundVolumeHierarchy::BoundSetup(std::shared_ptr<BoundVolume> bv, Vector& center)
{
    this->bv = bv;
    this->center = center;

    diameter.x = abs(this->bv->d_max_vals[0] - this->bv->d_min_vals[0]);
    diameter.y = abs(this->bv->d_max_vals[1] - this->bv->d_min_vals[1]);
    diameter.z = abs(this->bv->d_max_vals[2] - this->bv->d_min_vals[2]);
}

BoundVolumeHierarchy::BoundVolumeHierarchy(Vector& diameter, Vector& center)
{
    this->center=center;
    this->diameter = diameter;
    for (int i=0;i<NUM_CHILDREN;i++)
    {
        this->children[i] = nullptr;
    }
    this->bv = nullptr;
}



void BoundVolumeHierarchy::insert_object(std::shared_ptr<GObject> tri, int depth)
{
    //base case
    if (depth >= MAX_DEPTH || (objects.size() == 0 && is_leaf))
    {
        objects.push_back(tri);
        return;
    }

    is_leaf = false;

    if (objects.size()==1 )
    {
        auto tmp_tri = objects[0];
        objects.clear();
        this->insert_object(tmp_tri, depth);
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

        children[key] = std::make_unique<BoundVolumeHierarchy>(diameter, center);
    }

    children[key]->insert_object(tri, depth+1);
}

std::shared_ptr<BoundVolume> BoundVolumeHierarchy::build_BVH()
{
    std::vector<std::shared_ptr<BoundVolume>> child_volumes;
    for (int i=0;i<NUM_CHILDREN;i++)
    {
        if (children[i]!= nullptr)
        {
            std::shared_ptr<BoundVolume> bv = children[i]->build_BVH();
            if (bv!= nullptr)
            //if child contains something...
            {
                child_volumes.push_back(bv);
            }
        }
    }

    if (child_volumes.size() == 0)
    //No child nodes... this is a leaf, possibly containing objects.
    {
        if(objects.size() == 0)
        //leaf with no objects? fuckin useless node aren't we? Return no bv.
        {
            this->bv = nullptr;
        }else
        {
            std::vector<std::shared_ptr<BoundVolume>> volumes;
            for (int i =0; i<objects.size();i++)
            {
                if (objects[i]->bv == nullptr)
                    //the objects we have no bv therefore must be mesh triangles that have been inserted?
                {
                    std::vector<Vector> vertices;

                    std::shared_ptr<Triangle> tri_pointer = std::reinterpret_pointer_cast<Triangle>(objects[i]);
                    vertices.push_back(tri_pointer->v[0]);
                    vertices.push_back(tri_pointer->v[1]);
                    vertices.push_back(tri_pointer->v[2]);

                    volumes.push_back(BoundVolume::compute_bound_volume(vertices));
                }
                else
                {
                    volumes.push_back(std::dynamic_pointer_cast<BoundVolume>(objects[i]->bv));
                }
            }
            this->bv = BoundVolume::compute_bound_volume(volumes);
        }
    }else
    //We have child bounded volumes, create bvh around this
    {
        this->bv = BoundVolume::compute_bound_volume(child_volumes);
    }

    return this->bv;
}


GObject::intersection BoundVolumeHierarchy::intersect(const Vector& src, const Vector& d, int depth) const
//potential speed up is by making another intersect function that just returns bool instead of intersection obj.
{
    GObject::intersection bv_inter = bv->intersect(src,d);

#ifdef DEBUG_BVH
    if (depth >= 2)
    {
        return bv_inter;
    }
#endif
    GObject::intersection best_inter = GObject::intersection();
    if(!bv_inter.obj_ref.expired())
    //we have an intersection...
    {
        double min_t = std::numeric_limits<double>::max();

        if(is_leaf)
        {
            for(int i=0; i<objects.size();i++)
            {
                GObject::intersection tmp = objects[i]->intersect(src, d);
                if(!tmp.obj_ref.expired())
                //there was a object intersection...
                {
                    if(tmp.t<min_t && tmp.t > 0)
                    {
                        min_t = tmp.t;
                        best_inter = tmp;
                    }
                }
            }
        }
        else
        {
            for (int i=0;i<NUM_CHILDREN;i++)
            {
                if (children[i] != nullptr)
                {
                    GObject::intersection tmp = children[i]->intersect(src, d, depth+1);
                    if(!tmp.obj_ref.expired())
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
