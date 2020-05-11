#include "GObjects/Plane.h"
#include <iostream>

Plane::Plane(): GObject()
{
}

Plane::~Plane()
{
    delete bv;
    bv = nullptr;
}

Plane::Plane(Vector pos, Vector n, double l, double w, Color c, double shininess, double reflectivity): GObject(c, pos, shininess, reflectivity),
n(normalise(n)),
w(w),
l(l)
{
}

GObject::intersection Plane::intersect(const Vector& src, const Vector& d)
{
    intersection bv_inter = ((BoundVolume*)this->bv)->intersect(src, d);
    //return bv_inter;
    if(bv_inter.obj_ref != nullptr)
    {
        intersection inter;
        inter.obj_ref =nullptr;
        double tmp = d.dot(n);

        if (tmp!=0)
        {
            inter.t = (position-src).dot(n)/(tmp);
            inter.obj_ref = this;
            inter.n = n;
        }
        return inter;
    }
    return bv_inter;
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

    n = mat.mult_vec(n, 0);
    u = mat.mult_vec(u, 0);
    v = mat.mult_vec(v, 0);
    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);

    xml.FindElem("emission");
    Color::deserialize(xml.GetSubDoc(), emission);
    this->bv = BoundVolume::compute_bound_volume(this);


}
