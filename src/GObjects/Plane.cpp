#include "GObjects/Plane.h"
#include <iostream>

Plane::Plane(): GObject()
{
}

Plane::Plane(Vector pos, Vector n, double l, double w, Color c, double shininess, double reflectivity): GObject(c, pos, shininess, reflectivity),
n(normalise(n)),
w(w),
l(l)
{
}

double Plane::intersect(const Vector& src, const Vector& d)
{
    double tmp = d.dot(n);
    if (tmp==0)
    {
        return -1;
    }else
    {
        double t = (position-src).dot(n)/(tmp);
        return t;
    }
}

Vector Plane::normal(const Vector& p)
//every point p on plane has same normal.
{
    return normalise(n);
}

void Plane::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    w = std::stod(xml.GetAttrib("w"));
    l = std::stod(xml.GetAttrib("l"));
    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    xml.IntoElem();

    xml.FindElem("normal");
    Vector::deserialize(xml.GetSubDoc(), n);

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);
}
