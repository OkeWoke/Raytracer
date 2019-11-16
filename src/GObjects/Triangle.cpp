#include "GObjects/Triangle.h"

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
    Vector AB = v1-v2;
    Vector AC = v3-v2;
    n = normalise(AB.cross(AC));
}

double Triangle::intersect(Vector src, Vector d)
{
    if (d.dot(n) != 0)
    {
        double t = -(src-v2).dot(n)/(d.dot(n));
        if(t > 0)
        {
            //interesection with plane
            Vector p = src+t*d;
            double a = ((v1-v2).cross(p-v2)).dot(n);
            double b = ((v3-v1).cross(p-v1)).dot(n);
            double c = ((v2-v3).cross(p-v3)).dot(n);
            if ((a>0 && b>0 && c>0) || (a<0 && b <0 && c < 0))
            {
                //intersection within triangle
                return t;
            }
        }
    }
    return -1;
     //intersection with plane
}

Vector Triangle::normal(Vector p)
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
