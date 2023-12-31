#include "deserialize.h"
#include "ObjReader.h"

MeshConfig deserializeMesh(std::basic_string<char> xmlStr)
{
    MeshConfig meshConfig;
    CMarkup xml(xmlStr);

    xml.FindElem();

    meshConfig.filename = xml.GetAttrib("filename");
    meshConfig.shininess = std::stod(xml.GetAttrib("shininess"));
    meshConfig.reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    meshConfig.brdf = std::stod(xml.GetAttrib("brdf"));
    xml.IntoElem();

    xml.FindElem("position");
    Vector::deserialize(xml.GetSubDoc(), meshConfig.position);
    meshConfig.mat = meshConfig.mat * Matrix::translate(meshConfig.position);

    xml.FindElem("x_rot");
    meshConfig.mat = meshConfig.mat * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("y_rot");
    meshConfig.mat = meshConfig.mat * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("z_rot");
    meshConfig.mat = meshConfig.mat * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("scale");
    meshConfig.mat = meshConfig.mat * Matrix::scale(std::stod(xml.GetAttrib("factor")));

    xml.FindElem("color");
    Color::deserialize(xml.GetSubDoc(), meshConfig.color);

    xml.FindElem("texture");
    meshConfig.texture_filename = xml.GetAttrib("filename");

    return meshConfig;
}

void deserialize(const std::string& filename,
                 Scene& scene)
//Deserialises the scene/config.xml, modifies global structures and vectors
{
    CMarkup xml;
    xml.Load(filename);

    xml.FindElem(); //config
    scene.config.threads_to_use = std::stoi(xml.GetAttrib("threads"));
    if (scene.config.threads_to_use < 1){scene.config.threads_to_use=1;}
    scene.config.max_reflections = std::stoi(xml.GetAttrib("max_reflections"));
    scene.config.spp = std::stoi(xml.GetAttrib("samplesPP"));
    scene.config.stretch = xml.GetAttrib("stretch");

    xml.FindElem(); //scene.camera
    Camera::deserialize(xml.GetSubDoc(),scene.cam);

    while(xml.FindElem())
    {
        std::string element = xml.GetTagName();
        if(element == "Sphere")
        {
            std::shared_ptr<Sphere> sp = std::make_shared<Sphere>();
            sp->deserialize(xml.GetSubDoc());
            scene.objects.push_back(sp);
        }
        else if(element == "Plane")
        {
            std::shared_ptr<Plane> pl = std::make_shared<Plane>();
            pl->deserialize(xml.GetSubDoc());
            scene.objects.push_back(pl);
        }
        else if(element == "Light")
        {
            Light l = Light();
            l.deserialize(xml.GetSubDoc());
            scene.lights.push_back(l);
            continue;
        }
        else if(element == "Mesh")
        {
            MeshConfig meshConfig = deserializeMesh(xml.GetSubDoc());
            std::string rootPath = RootPath;
            if(meshConfig.filename.find(".obj") != std::string::npos){
                std::string fullPath = rootPath + "/data/3dmodels/"+ meshConfig.filename;
                ObjContents obj = obj_reader(fullPath, meshConfig.mat);

                if(meshConfig.texture_filename.find(".png") != std::string::npos)
                {
                    meshConfig.texture_filename = rootPath + "/data/textures/" + meshConfig.texture_filename;
                }

                std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(obj.vertices, obj.triangles, meshConfig);
                scene.objects.push_back(mesh);
            }
        }
    }
    for(int i =0;i<scene.objects.size();i++)
    {
        if(scene.objects[i]->is_light())
        {
            scene.gLights.push_back(scene.objects[i]);
        }
    }
}