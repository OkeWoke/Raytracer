#include "Mesh.h"

Mesh::Mesh(std::vector<Vector>& vertices, std::vector<std::shared_ptr<Triangle>>& triangles,  const MeshConfig& config): bvh(vertices), vertices(vertices), triangles(triangles)
{
    for (auto tri: triangles)
    {
        tri->color = config.color;
        tri->shininess = config.shininess;
        tri->brdf = config.brdf;
        tri->reflectivity = config.reflectivity;

        bvh.insert_object(tri,0);
    }

    this->color = config.color;
    this->shininess = config.shininess;
    this->brdf = config.brdf;
    this->reflectivity = config.reflectivity;
    (void) bvh.build_BVH();
    this->bv = bvh.bv;
    texture = png::image< png::rgb_pixel >(config.texture_filename);
}

GObject::intersection Mesh::intersect(const Vector& src, const  Vector& d)
{
    intersection inter = bvh.intersect(src, d, 0);//priority_intersect(src,d,0);
    if (inter.color.b == -939)
    {
        int t_x  = int((texture.get_width()-1)*inter.color.r);
        int t_y = int((texture.get_height()-1)*inter.color.g);
        png::rgb_pixel pix = texture[t_y][t_x];
        inter.color = Color(pix.red, pix.green, pix.blue);
    }else
    {
        inter.color = this->color;
    }
    if(inter.obj_ref != nullptr){inter.obj_ref->brdf = brdf;}

    return inter;
}

Vector Mesh::get_random_point(double val, double val2)
{
    return Vector();
}
