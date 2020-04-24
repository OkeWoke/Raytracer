#include "GObjects/Mesh.h"
#include<iostream>

Mesh::Mesh()
{

}

Mesh::Mesh(std::string filename) //old constructor no longer supported
{
    obj_reader(filename);
}

Mesh::~Mesh()
{
    for (auto p : triangles)
    {
        delete p;
    }

    triangles.clear();
}

GObject::intersection Mesh::intersect(const Vector& src, const  Vector& d)
{
    intersection inter;
    intersection bv_inter = bv->intersect(src, d);

    if (bv_inter.t != -1)
    {

        int triangle_hit_index;

        double closest_t = std::numeric_limits<double>::max();
        for(unsigned int i = 0; i < triangles.size(); i++)
        {
            intersection tri_inter_tmp = triangles[i]->intersect(src, d, texture);
            __sync_fetch_and_add(&numRayTrianglesTests, 1);

            if(tri_inter_tmp.t > 0 && tri_inter_tmp.t < closest_t)
            {
                __sync_fetch_and_add(&numRayTrianglesIsect, 1);
                closest_t = tri_inter_tmp.t;
                triangle_hit_index = i;
                inter.n = tri_inter_tmp.n;
                inter.color = tri_inter_tmp.color;
            } //finds closest triangle to intersect
        }

        if (closest_t < std::numeric_limits<double>::max())
        {
            inter.obj_ref = triangles[triangle_hit_index];
            inter.t = closest_t;
            //inter.color = tri_inter_tmp.color;
        }
    }


    return inter;
}

void Mesh::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    std::string filename = xml.GetAttrib("filename");

    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);
    mat = mat * Matrix::translate(position);

    xml.FindElem("x_rot");
    mat = mat * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("y_rot");
    mat = mat * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("z_rot");
    mat = mat * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("scale");
    mat = mat * Matrix::scale(std::stod(xml.GetAttrib("factor")));

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);

    xml.FindElem("texture");
    std::string texture_filename = xml.GetAttrib("filename");

    obj_reader(filename);//must be called last here.
    texture = png::image< png::rgb_pixel >(texture_filename);
}

void Mesh::obj_reader(std::string filename)
//takes string file name of .obj file that is triangulated.
//loads triangles vector up
{
    std::vector<Vector> vt;
    std::vector<Vector> vn;

    std::ifstream file;
    try
    {
        file.open(filename);
    }catch(std::system_error)
    {
        std::cout<<"Error reading file" << std::endl;
    }
    if(!file)
    {
        std::cout << "Unable to open .obj file" << std::endl;
        return;
    }

    std::string line;
    while(getline(file, line))
    {
        double x, y, z;
        if(line.length() < 3){continue;}
        try
        {
            std::istringstream iss(line.substr(2,36));
            if (line.substr(0,2) == "v ")
            {
                iss >> x >> y >> z;
                vertices.push_back(mat.mult_vec(Vector(x,y,z),1));
            }else if (line.substr(0,2) == "vt")
            {
                iss >> x >> y;
                vt.push_back(Vector(x,y,0));
            }else if (line.substr(0,2) == "vn")
            {
                iss >> x >> y >> z;

                vn.push_back(mat.mult_vec(Vector(x,y,z),0));
            }else if (line.substr(0,2) == "f ")
            {
                auto vec_stoi = [](const std::vector<std::string>& vec )
                {

                    return std::vector<int>{stoi(vec[0]), stoi(vec[1]), stoi(vec[2])};
                };

                //this vector should be length 3...
                std::vector<std::string> face_points = Utility::split(line.substr(2,100)," ");
                std::vector<int> i0 = vec_stoi(Utility::split(face_points[0],"/"));
                std::vector<int> i1 = vec_stoi(Utility::split(face_points[1],"/"));
                std::vector<int> i2 = vec_stoi(Utility::split(face_points[2],"/"));
                Triangle* tri = new Triangle(vertices[i0[0]-1], vertices[i1[0]-1], vertices[i2[0]-1], vt[i0[1]-1], vt[i1[1]-1], vt[i2[1]-1],vn[i0[2]-1], vn[i1[2]-1], vn[i2[2]-1]);
                tri->position = Vector(0,0,0);
                tri->color = color;
                tri->shininess = shininess;
                tri->reflectivity = reflectivity;
                triangles.push_back(tri);
            }

        }catch(out_of_range)
        {
            std::cout << "Error reading .obj file!" << std::endl;
            return;
        }
    }
    std::cout << "Triangle count: " << triangles.size() << std::endl;
    bv = BoundVolume::compute_bound_volume(this->vertices);
}
