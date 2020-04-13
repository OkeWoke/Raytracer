#include "BoundVolume.h"

static const Vector plane_normals[7]= {Vector(1,0,0), Vector(0,1,0), Vector(0,0,1), Vector(sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,sqrt(3)/3,sqrt(3)/3), Vector(-sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3), Vector(sqrt(3)/3,-sqrt(3)/3,sqrt(3)/3)};


BoundVolume::BoundVolume()
{


}

BoundVolume* compute_bound_volume(Mesh &mesh)
{
    BoundVolume* bv = new BoundVolume();

    for(int i=0; i < 7; i++)
    {
        double d_min = std::numeric_limits<double>::max();
        double d_max = std::numeric_limits<double>::min();

        for(unsigned int i = 0; i < mesh.triangles.size(); i++)
        {
            //yes the below is ugly but what can I do without changing triangle class...

            double d_1 = mesh.triangles[i]->v1.dot((plane_normals[i]));
            double d_2 = mesh.triangles[i]->v2.dot((plane_normals[i]));
            double d_3 = mesh.triangles[i]->v3.dot((plane_normals[i]));

            double d = 0;  //overlapping
            // INEFFICIENCY, triangles will have overlapping vertexes....
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

double BoundVolume::intersect(const Vector& src, const Vector& d)
{
    return -2;
}

Vector BoundVolume::normal(const Vector& p)
{
    return Vector(0,0,0);
}
