#include "GObjects/Mesh.h"
#include<iostream>
Mesh::Mesh()
{
    //ctor
    this->bounding_sphere = Sphere(center, 5);
}

Mesh::Mesh(std::string filename)
{
    //loads in vertices/triangles
    //sets limits
    obj_reader(filename);

    for(int i =0;i<6;i++)
    {
        //limits[i] =NULL;
    }

    this->bounding_sphere = Sphere(center, 5); //to be removed with BVH
}

Mesh::~Mesh()
{
    for (auto p : triangles)
    {
     delete p;
    }
    triangles.clear();
}

double Mesh::intersect(const Vector& src, const  Vector& d)
//to be overwritten
{

    if(bounding_sphere.intersect(src, d) != -1)
    {

        int triangle_hit_index;
        //std::cout << "Mesh bounding sphere hit" << std::endl;

        double closest_t = std::numeric_limits<double>::max();
        for(unsigned int i = 0; i < triangles.size(); i++)
        {

            double tmp = triangles[i]->intersect(src, d);
            if(tmp > 0 && tmp < closest_t)
            {
                closest_t = tmp;
                triangle_hit_index = i;
                //
                //std::cout << "Mesh hit, trignale found " << tmp << std::endl;
            } //finds closest triangle to intersect
        }
        if (closest_t < std::numeric_limits<double>::max())
        {
            tri = triangles[triangle_hit_index];
            return closest_t;
        }
    }

    return -1;
}

Vector Mesh::normal(const Vector& p)
{
    return tri->normal(p);
}

void Mesh::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    std::string filename = xml.GetAttrib("filename");
    obj_reader(filename);

    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), position);

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), color);
}

void Mesh::obj_reader(std::string filename)
//takes string file name of .obj file that is triangulated.
//loads triangles vector up
{
    //assume .obj is triangulated
    std::vector<Vector> v;
    std::vector<Vector> vt;
    std::vector<Vector> vn;

    std::ifstream file;
    file.open(filename);
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
                v.push_back(Vector(x,y,z));
            }else if (line.substr(0,2) == "vt")
            {
                iss >> x >> y >> z;
                vt.push_back(Vector(x,y,z));
            }else if (line.substr(0,2) == "vn")
            {
                iss >> x >> y >> z;
                vn.push_back(Vector(x,y,z));
            }else if (line.substr(0,2) == "f ")
            {
                //cout << line <<endl;
                auto vec_stoi = [](const std::vector<std::string>& vec )
                {

                    return std::vector<int>{stoi(vec[0]), stoi(vec[1]), stoi(vec[2])};
                };

                //this vector should be length 3...
                std::vector<std::string> face_points = Utility::split(line.substr(2,100)," ");
                std::vector<int> i0 = vec_stoi(Utility::split(face_points[0],"/"));
                std::vector<int> i1 = vec_stoi(Utility::split(face_points[1],"/"));
                std::vector<int> i2 = vec_stoi(Utility::split(face_points[2],"/"));
                Triangle* tri = new Triangle(v[i0[0]-1], v[i1[0]-1], v[i2[0]-1], vt[i0[1]-1], vt[i1[1]-1], vt[i2[1]-1],vn[i0[2]-1], vn[i1[2]-1], vn[i2[2]-1]);
                triangles.push_back(tri);
            }
        }catch(out_of_range)
        {
            std::cout << "Error reading .obj file!" << std::endl;
            return;
        }
    }
    std::cout << "Triangle count: " << triangles.size() << std::endl;
}

Mesh* Mesh::get_obj()
{
    return this;
}
