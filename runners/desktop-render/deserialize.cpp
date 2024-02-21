#include "deserialize.h"
#include "ObjReader.h"

MeshConfig deserializeMesh(std::basic_string<char> xmlStr)
{
    MeshConfig meshConfig;
    CMarkup xml(xmlStr);

    xml.FindElem();

    meshConfig.filename = xml.GetAttrib("filename");
    meshConfig.brdf = GObject::brdf_from_string(xml.GetAttrib("brdf"));

    if(meshConfig.brdf == GObject::BRDF::MIRROR)
    {
        meshConfig.reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    }
    xml.IntoElem();

    xml.FindElem("position");
    meshConfig.position = deserializeVector(xml.GetSubDoc());
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
    meshConfig.color = deserializeColor(xml.GetSubDoc());

    xml.FindElem("texture");
    meshConfig.texture_filename = xml.GetAttrib("filename");

    return meshConfig;
}

Scene deserializeScene(const std::string& filename)
//Deserialises the scene/config.xml, modifies global structures and vectors
{
    Scene scene;
    CMarkup xml;
    xml.Load(filename);

    xml.FindElem(); //config
    scene.config.threads_to_use = std::stoi(xml.GetAttrib("threads"));
    if (scene.config.threads_to_use < 1){scene.config.threads_to_use=1;}
    scene.config.max_reflections = std::stoi(xml.GetAttrib("max_reflections"));
    scene.config.spp = std::stoi(xml.GetAttrib("samplesPP"));
    scene.config.stretch = xml.GetAttrib("stretch");

    xml.FindElem(); //scene.camera
    scene.cam = deserializeCamera(xml.GetSubDoc());

    while(xml.FindElem())
    {
        std::string element = xml.GetTagName();
        if(element == "Sphere")
        {
            std::shared_ptr<Sphere> sp = std::make_shared<Sphere>(deserializeSphere(xml.GetSubDoc()));
            scene.objects.push_back(sp);
        }
        else if(element == "Plane")
        {
            std::shared_ptr<Plane> pl = std::make_shared<Plane>(deserializePlane(xml.GetSubDoc()));
            scene.objects.push_back(pl);
        }
        else if(element == "Light")
        {
            Light l = deserializeLight(xml.GetSubDoc());
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

    return scene;
}

Vector deserializeVector(const std::string& sub)
{
    Vector vec;
    CMarkup xml(sub);
    xml.FindElem();
    vec.x = std::stod(xml.GetAttrib("x"));
    vec.y = std::stod(xml.GetAttrib("y"));
    vec.z = std::stod(xml.GetAttrib("z"));

    return vec;
}

Color deserializeColor(const std::string& sub)
{
    Color color;
    CMarkup xml(sub);
    xml.FindElem();
    color.r = std::stod(xml.GetAttrib("r"));
    color.g = std::stod(xml.GetAttrib("g"));
    color.b = std::stod(xml.GetAttrib("b"));

    return color;
}

Sphere deserializeSphere(const std::string& strSubDoc)
{
    Sphere sphere;
    CMarkup xml(strSubDoc);

    xml.FindElem();
    sphere.radius = std::stod(xml.GetAttrib("radius"));
    sphere.brdf = GObject::brdf_from_string(xml.GetAttrib("brdf"));

    if(sphere.brdf == GObject::BRDF::MIRROR)
    {
        sphere.reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    }

    xml.IntoElem();

    xml.FindElem("position");
    sphere.position = deserializeVector(xml.GetSubDoc());

    xml.FindElem("color");
    sphere.color = deserializeColor(xml.GetSubDoc());

    if(xml.FindElem("emission"))
    {
        sphere.emission = deserializeColor(xml.GetSubDoc());
    }

    return sphere;
}

Light deserializeLight(const std::string& strSubDoc)
{
    Light light;
    CMarkup xml(strSubDoc);

    xml.FindElem();
    xml.IntoElem();

    xml.FindElem("position");
    light.position = deserializeVector(xml.GetSubDoc());

    xml.FindElem("color");
    light.color = deserializeColor(xml.GetSubDoc());

    return light;
}

Camera deserializeCamera(const std::string& sub)
{
    Camera cam;
    CMarkup xml(sub);

    xml.FindElem();
    cam.H = std::stod(xml.GetAttrib("w"));
    cam.V = std::stod(xml.GetAttrib("h"));
    cam.H_RES = std::stoi(xml.GetAttrib("H_Res"));
    cam.V_RES = std::stoi(xml.GetAttrib("V_Res"));

    cam.N = std::stod(xml.GetAttrib("fl"));
    cam.aperture = std::stod(xml.GetAttrib("aperture"));
    cam.focus_dist = std::stod(xml.GetAttrib("focus_dist"));

    xml.IntoElem();

    xml.FindElem("position");
    cam.pos = deserializeVector(xml.GetSubDoc());

    xml.FindElem("x_rot");

    Matrix tmp = cam.mat; //make sure its a carbon copy, not a reference.
    tmp = tmp * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("y_rot");
    tmp = tmp * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("z_rot");
    tmp = tmp * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));

    tmp = tmp * Matrix::translate(cam.pos);
    cam.n = normalise(Vector(tmp.ar[2][0], tmp.ar[2][1], tmp.ar[2][2]));
    cam.u = normalise(Vector(tmp.ar[1][0], tmp.ar[1][1], tmp.ar[1][2]));
    cam.v = normalise(Vector(tmp.ar[0][0], tmp.ar[0][1], tmp.ar[0][2]));

    return cam;
}

Plane deserializePlane(const std::string& strSubDoc)
{
    Plane plane;
    CMarkup xml(strSubDoc);

    xml.FindElem();
    plane.w = std::stod(xml.GetAttrib("w"));
    plane.l = std::stod(xml.GetAttrib("l"));
    plane.brdf = GObject::brdf_from_string(xml.GetAttrib("brdf"));

    if(plane.brdf == GObject::BRDF::MIRROR)
    {
        plane.reflectivity = std::stod(xml.GetAttrib("reflectivity"));
    }
    xml.IntoElem();

    xml.FindElem("position");
    plane.position = deserializeVector(xml.GetSubDoc());

    double tmp[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    plane.n = Vector(0,1,0);
    plane.u = Vector(1,0,0);
    plane.v = Vector(0,0,1);

    Matrix mat(tmp);

    xml.FindElem("x_rot");
    mat = mat * Matrix::rot_x(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("y_rot");
    mat = mat * Matrix::rot_y(std::stod(xml.GetAttrib("angle")));

    xml.FindElem("z_rot");
    mat = mat * Matrix::rot_z(std::stod(xml.GetAttrib("angle")));

    plane.n = normalise(mat.mult_vec(plane.n, 0));
    plane.u = normalise(mat.mult_vec(plane.u, 0));
    plane.v = normalise(mat.mult_vec(plane.v, 0));
    xml.FindElem("color");
    plane.color = deserializeColor(xml.GetSubDoc());

    if(xml.FindElem("emission"))
    {
        plane.emission = deserializeColor(xml.GetSubDoc());
    }
    return plane;
}



