#include "GObjects/Triangle.h"
#include<iostream>
Triangle::Triangle(): GObject()
{
    //ctor
}

Triangle::Triangle(Vector p1, Vector p2, Vector p3, Vector vt1, Vector vt2, Vector vt3, Vector vn1, Vector vn2, Vector vn3): GObject(),
v1(p1),
v2(p2),
v3(p3),
vt1(vt1),
vt2(vt2),
vt3(vt3),
vn1(vn1),
vn2(vn2),
vn3(vn3)
{
    Vector AB = v2-v1;
    Vector AC = v3-v1;
    n = normalise(AB.cross(AC)); //face normal
    //std::cout << "Triangle Normal Vec: " << n.to_string() << std::endl;
    position = Vector(0,0,0);
    color = Color(200,0,0);
    this->shininess = 200;
    this->reflectivity = 0.6;
    //com = p1 + p2 + p3
}

double Triangle::intersect(const Vector& src, const Vector& d)
{
    //std::cout<<"A" <<std::endl;
    if (d.dot(n) != 0)
    {
        double t = -(src-v1).dot(n)/(d.dot(n));
        //interesection with plane
        Vector p = src+t*d;
        double a = ((v2-v1).cross(p-v1)).dot(n);
        double b = ((v3-v2).cross(p-v2)).dot(n);
        double c = ((v1-v3).cross(p-v3)).dot(n);
        if ((a>=0 && b>=0 && c>=0) || (a<0 && b <0 && c < 0))
        {
            //intersection within triangle
            //std::cout << "triangle hit succesfully" << std::endl;
            return t;
        }

    }
    return -1;
     //intersection with plane
}

Vector Triangle::normal(const Vector& p)
{
    //placeholders
    return n;
}

std::string Triangle::to_string()
{
    std::ostringstream oss;
	oss << "(" << this->v1.to_string() << ", " << this->vt1.to_string() <<", " << this->vn3.to_string() << ")";
	return oss.str();
}

void Triangle::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    xml.IntoElem();

    xml.FindElem("v1");
    Vector::deserialize(xml.GetSubDoc(), v1);

    xml.FindElem("v2");
    Vector::deserialize(xml.GetSubDoc(), v2);

    xml.FindElem("v3");
    Vector::deserialize(xml.GetSubDoc(), v3);

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);

    Vector AB = v2-v1;
    Vector AC = v3-v1;
    n = normalise(AB.cross(AC));
}
