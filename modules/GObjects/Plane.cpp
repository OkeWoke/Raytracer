#include "Plane.h"
#include <iostream>
#include "BoundVolume.h"

Plane::Plane(): GObject()
{
}

Plane::~Plane()
{

}

Plane::Plane(Vector pos, Vector n, double l, double w, Color c, double shininess, double reflectivity): GObject(c, pos, shininess, reflectivity),
n(normalise(n)),
w(w),
l(l)
{
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

void Plane::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    w = std::stod(xml.GetAttrib("w"));
    l = std::stod(xml.GetAttrib("l"));
    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    brdf = std::stod(xml.GetAttrib("brdf"));
    xml.IntoElem();

    //xml.FindElem("normal");
    //Vector::deserialize(xml.GetSubDoc(), n);

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);
    //mat = mat * Matrix::translate(position);

    double tmp[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    n = Vector(0,1,0);
    u = Vector(1,0,0);
    v = Vector(0,0,1);

    Matrix mat(tmp);

    xml.FindElem("x_rot");
    mat = mat * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("y_rot");
    mat = mat * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("z_rot");
    mat = mat * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));

    n = normalise(mat.mult_vec(n, 0));
    u = normalise(mat.mult_vec(u, 0));
    v = normalise(mat.mult_vec(v, 0));
    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);

    xml.FindElem("emission");
    Color::deserialize(xml.GetSubDoc(), emission);
    this->bv = BoundVolume::compute_bound_volume(this);
}
