#include "Plane.h"
#include <iostream>
#include "BoundVolume.h"
#include "Markup-w.h"

Plane::Plane(): GObject()
{

}

//copy constructor
Plane::Plane(const Plane& p): GObject(p.color, p.position, p.shininess, p.reflectivity, p.emission, p.brdf)
{
    this->n = p.n;
    this->u = p.u;
    this->v = p.v;
    this->l = p.l;
    this->w = p.w;
    this->bv = BoundVolume::compute_bound_volume(p);
}

Plane::~Plane()
{

}

Plane::Plane(Vector pos,
             Vector n,
             double l,
             double w,
             Color c,
             double shininess,
             double reflectivity,
             int brdf,
             Color emission): GObject(c, pos, shininess, reflectivity, emission, brdf),
n(normalise(n)),
w(w),
l(l)
{
    this->bv = BoundVolume::compute_bound_volume(*this);
}

GObject::intersection Plane::intersect(const Vector& src, const Vector& d)
{
    intersection bv_inter = ((BoundVolume*)this->bv.get())->intersect(src, d);
    //return bv_inter;
    if(bv_inter.obj_ref != nullptr)
    {
        intersection inter = GObject::intersection();
        double tmp = d.dot(n);
        if (tmp!=0)
        {
            double tmp_t = (position-src).dot(n)/(tmp);
            Vector hit_point = src+tmp_t*d;
            if((hit_point-position).abs() < w)
            {
                inter.t = tmp_t;
                inter.obj_ref = this;
                inter.n = n;
            }

        }
        return inter;
    }
    return GObject::intersection();//bv_inter;
}

Vector Plane::get_random_point(double val1, double val2)
//Supplies a random point on the planes surface.
{
    Vector point = this->position;
    point = point + w*this->u*(val1-0.5) + l*this->v*(val2-0.5);
    return point;
}
