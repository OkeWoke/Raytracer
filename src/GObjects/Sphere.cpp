#include "GObjects/Sphere.h"

Sphere::Sphere(): GObject()
{
}

Sphere::~Sphere()
{
    delete this->bv;
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
    //intersection bv_inter = ((BoundVolume*)this->bv)->intersect(src, d);
    //if(bv_inter.obj_ref != nullptr)
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
            inter.obj_ref = this;
            inter.n = normalise(src+inter.t*d-position);
        }

        return inter;
    }
    //return bv_inter;
}

void Sphere::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    radius = std::stod(xml.GetAttrib("radius"));
    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);
    this->bv = BoundVolume::compute_bound_volume(this);

}
