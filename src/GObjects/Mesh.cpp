#include "GObjects/Mesh.h"
#include<iostream>

Mesh::Mesh()
{

}


Mesh::~Mesh()
{

    for (auto p : triangles)
    {
        delete p;
        p = nullptr;
    }
    triangles.clear();
    vertices.clear();
    delete bvh;
    bvh = nullptr;
    delete bv;
    bv = nullptr;

}

GObject::intersection Mesh::intersect(const Vector& src, const  Vector& d)
{
    intersection inter = bvh->intersect(src, d, 0);//priority_intersect(src,d,0);
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
    if(inter.obj_ref != nullptr){inter.obj_ref->brdf = 0;}

    return inter;
}

void Mesh::deserialize(std::string strSubDoc)
{
    CMarkup xml(strSubDoc);

    xml.FindElem();
    std::string filename = xml.GetAttrib("filename");

    shininess = std::stod(xml.GetAttrib("shininess"));
    reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    brdf = std::stod(xml.GetAttrib("brdf"));
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
                //tri->position = Vector(0,0,0);
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
    bv = BoundVolume::compute_bound_volume(this->vertices); //this is deleted by bvh destructor?
    Vector center = Vector(0,0,0);
    for(unsigned int k = 0; k < vertices.size(); k++)
    {
        center  = center + vertices[k];
    }
    center = center / vertices.size();
    this->bvh = new BoundVolumeHierarchy(bv, center);
    for (auto tri: triangles)
    {
        bvh->insert_object(tri,0);
    }
    auto aaa = bvh->build_BVH();
    //delete aaa;
    //aaa =nullptr;//deleting bvh will delete this.
}

Vector Mesh::get_random_point(double val, double val2)
{
    return Vector();
}
