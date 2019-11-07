#include "Sphere.h"

Sphere::Sphere(): GObject()
{
    //
}

Sphere::Sphere(Vector pos, double radius, Color a, Color d, Color s, double shininess): GObject()
{
    ambient = a;
    diffuse = d;
    specular = s;
    this->radius = radius;
    position = pos;
    this->shininess = shininess;
}

double Sphere::intersect(Vector src, Vector d)
{
    src-=position;
    double a = d.dot(d);
    double b = 2*d.dot(src);
    double c = src.dot(src) - (radius*radius);

    double disc = (b*b)-(4*a*c);
    if(disc > 0)
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
            return t_1;
        }else
        {
            return t_2;
        }

    }else
    {
        return -1;
    }
}

Vector Sphere::normal(Vector p)
{
    return normalise(p-position);
}
