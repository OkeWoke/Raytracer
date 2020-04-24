#include "BoundVolume.h"

const Vector BoundVolume::plane_normals[7]= {Vector(1,0,0), Vector(0,1,0), Vector(0,0,1), Vector(sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3), Vector(sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3)};

BoundVolume::BoundVolume()
{
}

BoundVolume* BoundVolume::compute_bound_volume(std::vector<Vector>& vertices)
{
    BoundVolume* bv = new BoundVolume();
    bv->color = Color(0,255,0);

    for(int i=0; i < 7; i++)
    {
        double d_min = std::numeric_limits<double>::max();
        double d_max = std::numeric_limits<double>::min();

        for(unsigned int k = 0; k < vertices.size(); k++)
        {

            double d = vertices[k].dot((BoundVolume::plane_normals[i]));

            if(d>d_max)
            {
                d_max = d;
            }
            if(d<d_min)
            {
                d_min = d;
            }
        }

        bv->d_min_vals[i] = d_min;
        bv->d_max_vals[i] = d_max;
    }

    return bv;
}

BoundVolume* BoundVolume::compute_bound_volume(std::vector<BoundVolume*> volumes)
//compute a BV around a collection of several BV...
{
    //since all bv share the same vectors, I guess we just compare d_max and d-min vals or soemthing? and get the most bounding of em all? yee

}

GObject::intersection BoundVolume::intersect(const Vector& src, const Vector& d)
{
    double largest_t_near = std::numeric_limits<double>::min();;
    double smallest_t_far = std::numeric_limits<double>::max();
    GObject::intersection inter;
    int plane_index = 0;

    for(int i=0; i < 7; i++)
    {
        double d_dot_N = d.dot(this->plane_normals[i]);
        double src_dot_N = src.dot(this->plane_normals[i]);

        double d_val_min = d_min_vals[i];
        double d_val_max = d_max_vals[i];

        double t_near = (d_val_min - src_dot_N)/d_dot_N;
        double t_far = (d_val_max - src_dot_N)/d_dot_N;

        if (d_dot_N < 0 )
        {
            std::swap(t_near, t_far);
        }
        if (t_near > largest_t_near)
        {
            largest_t_near = t_near;
            plane_index = i;
        }

        if (t_far < smallest_t_far){smallest_t_far = t_far;}
        if(largest_t_near> smallest_t_far) return inter;

    }

    inter.t = largest_t_near;
    inter.n = this->plane_normals[plane_index];
    inter.obj_ref = this;

    return inter;
}
