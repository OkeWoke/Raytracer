#include "BoundVolumeHierarchy.h"

BoundVolumeHierarchy::BoundVolumeHierarchy( )
{
    //ctor
}

BoundVolumeHierarchy::BoundVolumeHierarchy(GObject* bv, Vector center)
{
    this->bv = (BoundVolume*)bv;
    this->center = center;

    diameter.x = abs(this->bv->d_max_vals[0] - this->bv->d_min_vals[0]);
    diameter.y = abs(this->bv->d_max_vals[1] - this->bv->d_min_vals[1]);
    diameter.z = abs(this->bv->d_max_vals[2] - this->bv->d_min_vals[2]);
    //std::cout<<diameter.to_string() <<std::endl;
    //std::cout<<center.to_string() << std::endl;

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
    objects.clear();
    for (auto p : children)
    {
        delete p;
    }
    delete bv;
}

void BoundVolumeHierarchy::insert_object(GObject* tri, int depth)
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

        children[key] = new BoundVolumeHierarchy(diameter, center);
    }

    children[key]->insert_object(tri, depth+1);
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
    //No child nodes... this is a leaf, possibly containing objects.
    {
        if(objects.size() == 0)
        //leaf with no objects? fuckin useless node aren't we? Return no bv.
        {
            this->bv = nullptr;
        }else
        {
            if (objects[0]->bv == nullptr)
            //the objects we have no bv therefore must be triangles that have been inserted?
            {
                std::vector<Vector> vertices;
                for (int i =0; i<objects.size();i++)
                {
                    Triangle* tri_pointer = (Triangle*)objects[i];
                    vertices.push_back(tri_pointer->v1);
                    vertices.push_back(tri_pointer->v2);
                    vertices.push_back(tri_pointer->v3);
                }
                this->bv = BoundVolume::compute_bound_volume(vertices);
            }else
            {
                std::vector<BoundVolume*> volumes;
                for(int i=0; i<objects.size();i++)
                {
                    BoundVolume* obj_bv_pointer = (BoundVolume*)objects[i]->bv;
                    volumes.push_back(obj_bv_pointer);
                }
                this->bv = BoundVolume::compute_bound_volume(volumes);
            }
            //at this point we could clear memory of objects as the instance doesn't need it anymore..., although...
            //although what?
        }
    }else
    //We have child bounded volumes, create bvh around this
    {
        this->bv = BoundVolume::compute_bound_volume(child_volumes);
    }

    return this->bv;
}

GObject::intersection BoundVolumeHierarchy::intersect(const Vector& src, const Vector& d, int depth)
//potential speed up is by making another intersect function that just returns bool instead of intersection obj.
{
    GObject::intersection bv_inter = GObject::intersection();
    if(this->bv == nullptr)
        //this case shouldnt happen?
    {
        return bv_inter;
    }

    bv_inter = bv->intersect(src,d);
    if (depth >= 2)
    {
        //return bv_inter;
    }
    GObject::intersection best_inter = GObject::intersection();
    if(bv_inter.obj_ref != nullptr)
    //we have an intersection...
    {
        double min_t = std::numeric_limits<double>::max();

        if(is_leaf)
        {
            for(int i=0; i<objects.size();i++)
            {
                GObject::intersection tmp = objects[i]->intersect(src, d);
                if(tmp.obj_ref != nullptr)
                //there was a object intersection...
                {
                    if(tmp.t<min_t && tmp.t > 0.001)
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
                    GObject::intersection tmp = children[i]->intersect(src, d, depth+1);
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

struct BVHInter
{
    BoundVolumeHierarchy* bvh;
    GObject::intersection inter = GObject::intersection();
};

static bool Compare(BVHInter inter1, BVHInter inter2)
{
    return inter1.inter.t > inter2.inter.t;
}


GObject::intersection BoundVolumeHierarchy::priority_intersect(const Vector& src, const Vector& d, int depth)
{

    std::priority_queue<BVHInter, std::vector<BVHInter>, std::function<bool(BVHInter, BVHInter)>> pq(Compare);
    for (int i=0; i<8; i++)
    {
        if (children[i] != nullptr)
        {
            GObject::intersection tmp = children[i]->bv_intersect(src, d);
            if(tmp.obj_ref != nullptr)
            {
                BVHInter bvh_tmp;
                bvh_tmp.inter = tmp;
                bvh_tmp.bvh = children[i];
                pq.push(bvh_tmp);
            }
        }
    }

    while(true)
    {
        BVHInter top_node = pq.top();
        pq.pop();
        if(top_node.bvh->is_leaf)
        {
            //below line possibly slow/inefficient?
            GObject::intersection leaf_inter = top_node.bvh->intersect(src, d, 0);//depth value incorrect here but oh well.
            if(leaf_inter.obj_ref != nullptr && leaf_inter.t < pq.top().inter.t)
            //we have an intersection with somethin inside leaf node...
            {
                return leaf_inter;
            }
        }else
        {
            for (int i=0;i<8;i++)
            {
                if (top_node.bvh->children[i] != nullptr)
                {
                    GObject::intersection tmp = top_node.bvh->children[i]->bv_intersect(src, d);
                    if(tmp.obj_ref != nullptr)
                    {
                        BVHInter bvh_tmp;
                        bvh_tmp.inter = tmp;
                        bvh_tmp.bvh = top_node.bvh->children[i];
                        pq.push(bvh_tmp);
                    }
                }
            }
        }
    }
}

GObject::intersection BoundVolumeHierarchy::bv_intersect(const Vector& src, const Vector& d)
{
    GObject::intersection inter = bv->intersect(src,d); //need to remove this check from normal intersect

    return inter;
}
