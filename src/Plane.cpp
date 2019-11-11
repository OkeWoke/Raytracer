#include "Plane.h"
#include <iostream>
Plane::Plane(): GObject()
{
//
}

Plane::Plane(Vector pos, Vector n, double l, double w, Color a, Color d, Color s, double shininess, double reflectivity): GObject()
{
    ambient = a;
    diffuse = d;
    specular = s;
    this->n = normalise(n);
    position = pos;
    this->shininess = shininess;
    this->reflectivity = reflectivity;
    this->w = w;
    this->l = l;
}

double Plane::intersect(Vector src, Vector d)
{
    if (d.dot(n)==0)
    {
        return -1;
    }else
    {
        //std::cout <<"plane hit" <<std::endl;
        double t = (position.abs()-src.dot(n))/(d.dot(n));
        return t;
    }
}

Vector Plane::normal(Vector p)
//every point p on plane has same normal.
{
    return n;
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

    xml.FindElem("ambient");
    Color::deserialize(xml.GetSubDoc(), ambient);

    xml.FindElem("diffuse");
    Color::deserialize(xml.GetSubDoc(), diffuse);

    xml.FindElem("specular");
    Color::deserialize(xml.GetSubDoc(), specular);
}
