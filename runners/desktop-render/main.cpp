#include <iostream>
#include <sstream>
#include <iomanip>

#include <string>
#include <pngpp-w.hpp>
#include "CImg-w.h"

#include <functional>

#include <time.h>

#include "render.h"
#include "Camera.h"
#include "Light.h"

#include "BoundVolumeHierarchy.h"
#include "HaltonSampler.h"
#include "RandomSampler.h"
#include "Sphere.h"
#include "GObject.h"
#include "Plane.h"
#include "Mesh.h"
#include "Markup-w.h"
#include "stats.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// function prototypes
void draw(ImageArray& img, const std::string& filename, const std::string& stretch);
void deserialize(std::string filename, std::vector<Light>& lights, std::vector<GObject*>& gLights, std::vector<GObject*>& objects, Camera& cam, Config& config);

extern Stats stats;

struct Scene
{
    std::vector<GObject*> objects;
    std::vector<Light> lights;
    std::vector<GObject*> gLights; //GObjects that have emission.
    Camera cam;
    Config config;

    ~Scene()
    {
        for (auto p : objects)
        {
            delete p;
            p = nullptr;
        }

        objects.clear();
        lights.clear();
    }
};

int main()
{
    std::cout<<"Loading scene from scene.xml..." << std::endl;
    auto load_start = std::chrono::steady_clock::now();
    std::string rootPath = RootPath;
    std::string renderDest = rootPath + "/renders/";

    Scene scene;
    deserialize(rootPath + "/data/scenes/scene.xml", scene.lights, scene.gLights, scene.objects, scene.cam, scene.config);

    auto load_end = std::chrono::steady_clock::now();
    std::cout<<"Loading completed in: " << (load_end-load_start)/std::chrono::milliseconds(1)<< " (ms)" << std::endl;

    ImageArray img(scene.cam.H_RES, scene.cam.V_RES);
    cimg_library::CImg<float> display_image(scene.cam.H_RES, scene.cam.V_RES,1,3,0);
    cimg_library::CImgDisplay display(display_image, "Oke's Path Tracer!");

    //Creation of BoundVolume Hierarchy
    auto bvh_start = std::chrono::steady_clock::now();
    BoundVolumeHierarchy bvh = BoundVolumeHierarchy(scene.objects);
    auto bvh_end = std::chrono::steady_clock::now();
    std::cout<<"BVH Constructed in: " << (bvh_end-bvh_start)/std::chrono::milliseconds(1)<< " (ms)" << std::endl;

    //Creation of samplers used for montecarlo integration.
    RandomSampler sampler1, sampler2;
    //HaltonSampler(7, rand()%5000 + 1503);
    //HaltonSampler(3, rand()%5000 + 5000);

    /////////////////////////////////////// CAST & DISPLAY  CODE /////////////////////////////
    auto cast_start = std::chrono::steady_clock::now();
    int s;
    double exponent = 1/4.0;
    for(s=0;s<scene.config.spp; s++)
    {
        cast_rays_multithread(scene.config, scene.cam, img, sampler1, sampler2, bvh, scene.objects, scene.lights, scene.gLights);
        for (int i = 0; i < img.HEIGHT * img.WIDTH; ++i)
        {
            int x = i % img.WIDTH;
            int y = i / img.WIDTH;
            display_image(x,y,0) = pow(img.pixelMatrix[i].r, exponent); // TODO: Verify this.
            display_image(x,y,1) = pow(img.pixelMatrix[i].g, exponent);
            display_image(x,y,2) = pow(img.pixelMatrix[i].b, exponent);

        }
        display_image.display(display);
        if (display.is_closed())
        {
            break;
        }
        img.floatArrayUpdate();
    }

    auto cast_end = std::chrono::steady_clock::now();

    const int orw = 20;
    std::cout << "Casting completed in: "<< std::setw(orw) << (cast_end - cast_start)/std::chrono::milliseconds(1)<< " (ms)"<<std::endl;
    std::cout << "Number of primary rays: " << std::setw(orw+1) << stats.numPrimaryRays << std::endl;
    std::cout << "Number of Triangle Tests: " << std::setw(orw) << stats.numRayTrianglesTests.load() << std::endl;
    std::cout << "Number of Triangle Intersections: " <<std::setw(orw-11) << stats.numRayTrianglesIsect.load() << std::endl;
    std::cout << "Percentage of sucesful triangle tests: " << std::setw(orw-12) << 100*(float) stats.numRayTrianglesIsect.load()/stats.numRayTrianglesTests.load()<< "%" << std::endl;
    std::cout << "----------------------------------------------------------\n\n\n\n"<<std::endl;
    std::cout << "Waiting for modification of scene.xml or close window to save" << std::endl;

    //Sample scaling, do not touch this as this ensures each image has same relative brightness regardless of no. samples.
    for (int i = 0; i < img.PIXEL_COUNT; ++i)
    {
        img.pixelMatrix[i] = img.pixelMatrix[i]/s;
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream filename;
    filename << renderDest << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    filename << "_spp-" << s <<"_cast-"<<(cast_end-cast_start)/std::chrono::seconds(1)<<".png";

    // Draw/Save code
    auto save_start = std::chrono::steady_clock::now();

    draw(img, filename.str(), scene.config.stretch);
    img.clearArray();
    auto save_end = std::chrono::steady_clock::now();
    std::cout << "Image Save completed in: "<< std::setw(orw-7) <<(save_end - save_start)/std::chrono::milliseconds(1)<< " (ms)"<<std::endl;
    std::cout << "----------------------------------------\n\n\n\n" << std::endl;


    //getch();
    return 0;

    /* Animation codes
    std::ostringstream filename;
    filename << "render" << setfill('0') <<std::setw(3)<< i << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";
      //system("D:\Programming\Raytracer\ffmpeg -f image2 -framerate 24 -i D:\Programming\Raytracer\renders\test%03d.png -pix_fmt yuv420p -b:v 0 -crf 30 -s 1000x1000 render2.webm");
    */
}



void draw(ImageArray& img, const std::string& filename, const std::string& stretch)
{
    if(stretch == "norm")
    {
        img.normalise(img.MAX_VAL);
    }else if(stretch == "gamma")
    {
        img.gammaCorrection(1.0/2.2);
        img.normalise(img.MAX_VAL);
    }else if(stretch == "rein")
    {
        img.normalise(1.0);
        img.reinhardToneMap();
        img.normalise(img.MAX_VAL);
    }
    png::image< png::rgb_pixel > image(img.WIDTH, img.HEIGHT);

    for (int y = 0; y < img.HEIGHT; ++y)
    {
        for (int x = 0; x < img.WIDTH; ++x)
        {
            Color c = img.pixelMatrix[img.index(x,y)];
            image[y][x] = png::rgb_pixel((int)c.r, (int)c.g,(int) c.b);
        }
    }

    image.write(filename);
}

void deserialize(std::string filename, std::vector<Light>& lights, std::vector<GObject*>& gLights, std::vector<GObject*>& objects, Camera& cam, Config& config)
//Deserialises the scene/config.xml, modifies global structures and vectors
{
    CMarkup xml;
    xml.Load(filename);

    xml.FindElem(); //config
    config.threads_to_use = std::stoi(xml.GetAttrib("threads"));
    if (config.threads_to_use < 1){config.threads_to_use=1;}
    config.max_reflections = std::stoi(xml.GetAttrib("max_reflections"));
    config.spp = std::stoi(xml.GetAttrib("samplesPP"));
    config.stretch = xml.GetAttrib("stretch");

    xml.FindElem(); //camera
    Camera::deserialize(xml.GetSubDoc(),cam);

    while(xml.FindElem())
    {
        std::string element = xml.GetTagName();
        if(element == "Sphere")
        {
            Sphere* sp = new Sphere();
            sp->deserialize(xml.GetSubDoc());
            objects.push_back(sp);
        }
        else if(element == "Plane")
        {
            Plane* pl = new Plane();
            pl->deserialize(xml.GetSubDoc());
            objects.push_back(pl);
        }
        else if(element == "Light")
        {
            Light l = Light();
            l.deserialize(xml.GetSubDoc());
            lights.push_back(l);
            continue;
        }
        else if(element == "Mesh")
        {
            Mesh* m = new Mesh();
            m->deserialize(xml.GetSubDoc());
            objects.push_back(m);
        }
    }
    for(int i =0;i<objects.size();i++)
    {
        if(is_light(objects[i]))
        {
            gLights.push_back(objects[i]);
        }
    }
}
