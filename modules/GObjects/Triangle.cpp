#include <iostream>
#include <atomic>

#include "Triangle.h"
#include "stats.h"
#include "Markup-w.h"

Triangle::Triangle(): GObject()//constructor no longer supported
{
    //ctor
}

Triangle::Triangle(Vector v[3], Vector vt[3], Vector vn[3]): GObject(),
v {v[0], v[1], v[2]},
vt {vt[0], vt[1], vt[2]},
vn {vn[0], vn[1], vn[2]}
{
    AB = v[1]-v[0];
    AC = v[2]-v[0];
    n = AB.cross(AC);
    area = n.dot(n);
    position = (v[0]+v[1]+v[2])/3;
}

GObject::intersection Triangle::intersect(const Vector& src, const Vector& d)
{
    stats.numRayTrianglesTests.fetch_add(1); // __sync_fetch_and_add(&numRayTrianglesTests, 1);
    intersection inter = intersection();
    double d_dot_n = d.dot(n);

    if (d_dot_n  !=0) //normal is pointing outward and not perpendicular to incoming ray
    {
        double t = -(src-v[0]).dot(n)/d_dot_n;

        //interesection with plane
        Vector p = src+t*d;
        double bary_u = ((AB).cross(p-v[0])).dot(n);
        double bary_v = ((v[2]-v[1]).cross(p-v[1])).dot(n);
        double c = ((-1*AC).cross(p-v[2])).dot(n);
        if ((bary_u>=0 && bary_v>=0 && c>=0) || (bary_u<0 && bary_v <0&& c < 0))
        {
            //intersection within triangle
            //__sync_fetch_and_add(&numRayTrianglesIsect, 1);
            stats.numRayTrianglesIsect.fetch_add(1);
            bary_u/=area;
            bary_v/=area;
            double bary_w = 1 - bary_u - bary_v;
            if (vn[0] == Vector()) {
                inter.n = normalise(n);
            }else
            {
                inter.n = normalise(vn[0]*bary_v + vn[1]*bary_w + vn[2]*bary_u);
            }

            inter.t = t;
            inter.obj_ref = shared_from_this();
            Vector tc =  ((bary_v*vt[0] + bary_w*vt[1] + bary_u*vt[2]));
            double t_x = fmod(tc.x,1);
            double t_y = fmod(tc.y, 1);
            if (t_x<0){t_x++;}
            if (t_y<0){t_y++;}
            inter.color = Color(t_x, t_y, -99);
        }
    }

    return inter;
}

std::string Triangle::to_string()
{
    std::ostringstream oss;
	oss << "(" << this->v[0].to_string() << ", " << this->vt[0].to_string() <<", " << this->vn[2].to_string() << ")";
	return oss.str();
}

Vector Triangle::get_random_point(double val, double val2)
{
    return Vector();
}
