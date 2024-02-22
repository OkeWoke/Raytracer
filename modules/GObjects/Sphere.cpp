#include "Sphere.h"
#include "Markup-w.h"
#include "BoundVolume.h"
#include <memory>

Sphere::Sphere(): GObject()
{
}

Sphere::~Sphere()
{

}

//copy constructor
Sphere::Sphere(const Sphere& s): GObject(s.color, s.position, s.shininess, s.reflectivity, s.emission, s.brdf)
{
    this->radius = s.radius;
    this->bv = BoundVolume::compute_bound_volume(s);
}

Sphere::Sphere(Vector pos, double radius): GObject(position),
radius(radius)
{
}

Sphere::Sphere(Vector pos, double radius, Color c, double shininess, double reflectivity): GObject(c, pos, shininess, reflectivity),
radius(radius)
{
}

GObject::intersection Sphere::intersect(const Vector& src, const Vector& d)
{
    intersection bv_inter = ((BoundVolume*)this->bv.get())->intersect(src, d);
    if(!bv_inter.obj_ref.expired())
    {
        intersection inter = intersection();

        Vector cen = src-position;
        double a = d.dot(d);
        double b = 2*d.dot(cen);
        double c = cen.dot(cen) - (radius*radius);

        double disc = (b*b)-(4*a*c);

        if(disc >= 0)
        {
            double t_1;
            if (b>0)
            {
                t_1 = (- b+ sqrt(disc) ) /( 2 * a);
            }else
            {
                t_1 = (- b -sqrt(disc) ) /( 2 * a);
            }
            double t_2 = c/(a*t_1);

            if (t_1 < t_2)
            {
                inter.t = t_1;
            }else
            {
                inter.t = t_2;
            }
            inter.obj_ref = shared_from_this();
            inter.n = normalise(src+inter.t*d-position);
        }
        return inter;
    }
    return bv_inter;
}


Vector Sphere::get_random_point(double val1, double val2)
{
 return Vector(0,0,0);
}
