#include "GObjects/Triangle.h"
#include<iostream>

Triangle::Triangle(): GObject()//constructor no longer supported
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
    AB = v2-v1;
    AC = v3-v1;
    n = AB.cross(AC);
    area = n.dot(n);
    position = (p1+p2+p3)/3;
}

GObject::intersection Triangle::intersect(const Vector& src, const Vector& d)
{
    __sync_fetch_and_add(&numRayTrianglesTests, 1);
    intersection inter;
    double d_dot_n = d.dot(n);

    if (d_dot_n  <0) //normal is pointing outward and not perpendicular to incoming ray
    {
        double t = -(src-v1).dot(n)/d_dot_n;

        //interesection with plane
        Vector p = src+t*d;
        double bary_u = ((AB).cross(p-v1)).dot(n);
        double bary_v = ((v3-v2).cross(p-v2)).dot(n);
        double c = ((-1*AC).cross(p-v3)).dot(n);
        if ((bary_u>=0 && bary_v>=0 && c>=0) || (bary_u<0 && bary_v <0&& c < 0))
        {
            //intersection within triangle
            __sync_fetch_and_add(&numRayTrianglesIsect, 1);

            bary_u/=area;
            bary_v/=area;
            double bary_w = 1 - bary_u - bary_v;
            inter.n = normalise(vn1*bary_v + vn2*bary_w + vn3*bary_u);
            inter.t = t;
            inter.obj_ref = this;
            Vector tc =  ((bary_v*vt1 + bary_w*vt2 + bary_u*vt3));
            double t_x = fmod(tc.x,1);
            double t_y = fmod(tc.y, 1);
            if (t_x<0){t_x++;}
            if (t_y<0){t_y++;}
            inter.color = Color(t_x, t_y, -999);
        }
    }

    return inter;
}

std::string Triangle::to_string()
{
    std::ostringstream oss;
	oss << "(" << this->v1.to_string() << ", " << this->vt1.to_string() <<", " << this->vn3.to_string() << ")";
	return oss.str();
}

void Triangle::deserialize(std::string strSubDoc)//old method no longer working
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

    AB = v2-v1;
    AC = v3-v1;
    n = normalise(AB.cross(AC));
}
