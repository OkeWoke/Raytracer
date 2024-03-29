#include "BoundVolume.h"

const Vector BoundVolume::plane_normals[7]= {Vector(1,0,0), Vector(0,1,0), Vector(0,0,1), Vector(sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3), Vector(sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3)};


std::shared_ptr<BoundVolume> BoundVolume::compute_bound_volume(std::vector<Vector>& vertices)
{
    std::shared_ptr<BoundVolume> bv = std::make_shared<BoundVolume>();
    bv->color = Color(0,255,0);

    for(int i=0; i < 7; i++)
    {
        double d_min = std::numeric_limits<double>::max();
        double d_max = std::numeric_limits<double>::lowest();

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

std::shared_ptr<BoundVolume> BoundVolume::compute_bound_volume(std::vector<std::shared_ptr<BoundVolume>>& volumes)
//compute a BV around a collection of several BV...
{
    //since all bv share the same vectors, I guess we just compare d_max and d-min vals or soemthing? and get the most bounding of em all? yee
    // out of all max values
    std::shared_ptr<BoundVolume> new_bv = std::make_shared<BoundVolume>();
    new_bv->color = Color(0,255,0);

    //maximum
    for(int i=0; i<7; i++)
    {
        double d_min = std::numeric_limits<double>::max();
        double d_max = std::numeric_limits<double>::lowest();

        for (auto bv: volumes)
        {
            double d_min_tmp = bv->d_min_vals[i];
            double d_max_tmp = bv->d_max_vals[i];

            if (d_min_tmp < d_min)
            {
                d_min = d_min_tmp;
            }

            if (d_max_tmp > d_max)
            {
                d_max = d_max_tmp;
            }
        }

        new_bv->d_min_vals[i] = d_min;
        new_bv->d_max_vals[i] = d_max;
    }

    return new_bv;
}

std::shared_ptr<BoundVolume> BoundVolume::compute_bound_volume(std::vector<std::shared_ptr<GObject>>& objects)
// please note this is only for surface level objects, not meant for objects's bv, please cast objects bv to BoundVolume and call other method.
{
    std::vector<std::shared_ptr<BoundVolume>> volumes;
    for (int i=0;i<objects.size();i++)
    {
        auto ptr = objects[i]->bv;
        if (ptr == nullptr)
        {
            std::cout << "Warning: object " << i << " has no BV" << std::endl;
        }else
        {
            volumes.push_back(std::dynamic_pointer_cast<BoundVolume>( objects[i]->bv));
        }
    }
    return compute_bound_volume(volumes);
}

std::shared_ptr<BoundVolume> BoundVolume::compute_bound_volume(const Sphere& sphere)
//Compute a BV around a sphere
{
    std::shared_ptr<BoundVolume> new_bv = std::make_shared<BoundVolume>();
    new_bv->color = Color(0,255,0);

    for(int i=0; i<7; i++)
    {
        new_bv->d_min_vals[i] = (sphere.position - sphere.radius*BoundVolume::plane_normals[i]).dot((BoundVolume::plane_normals[i]));
        new_bv->d_max_vals[i] = (sphere.position + sphere.radius*BoundVolume::plane_normals[i]).dot((BoundVolume::plane_normals[i]));
    }

    return new_bv;
}

std::shared_ptr<BoundVolume> BoundVolume::compute_bound_volume(const Plane& plane)
{
    Vector h_offset = plane.u*plane.w;
    Vector v_offset = plane.v*plane.l;

    Vector v1 = plane.position + h_offset  + v_offset + plane.n*0.1;
    Vector v2 = v1 - 2*v_offset;
    Vector v3 = plane.position - h_offset - v_offset + plane.n*0.1;
    Vector v4 = v3 +2*v_offset;

    std::vector<Vector> vertices{v1, v2, v3, v4};
    std::vector<Vector> vertices_to_send;

    for (Vector vec : vertices)
    {
        vertices_to_send.push_back(vec);
        vertices_to_send.push_back(vec - 0.2*plane.n);
    }

   return compute_bound_volume(vertices_to_send);
}

GObject::intersection BoundVolume::intersect(const Vector& src, const Vector& d)
{
    double largest_t_near = std::numeric_limits<double>::lowest();
    double smallest_t_far = std::numeric_limits<double>::max();
    GObject::intersection inter = GObject::intersection();
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
    inter.obj_ref = (GObject*)this;

    return inter;
}

Vector BoundVolume::get_random_point(double val, double val2)
{
    return Vector(0,0,0);
}
