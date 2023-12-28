#include <iostream>
#include <sstream>
#include <iomanip>

//#include <math.h>
#include <string>
#include <pngpp-w.hpp>
#include "CImg-w.h"

#include <functional>

#include <time.h>

#include "render.h"
#include "Camera.h"
#include "Light.h"

#include "Vector.h"

#include "BoundVolume.h"
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
void draw(ImageArray& img, std::string filename);
void deserialize(std::string filename, std::vector<Light>& lights, std::vector<GObject*>& gLights, std::vector<GObject*>& objects, Camera& cam, Config& config);


extern Stats stats;

int main()
{
    std::vector<GObject*> objects;
    std::vector<Light> lights;
    std::vector<GObject*> gLights; //GObjects that have emission.

    Camera cam;
    Config config;
    int orw = 20;

    //initial call to deserialize just so I can define ImageArray...
    std::cout<<"Loading scene from scene.xml..." << std::endl;
    auto load_start = std::chrono::steady_clock::now();
    std::string rootPath = RootPath;
    std::string renderDest = rootPath + "/renders/";

    deserialize(rootPath + "/data/scenes/scene.xml", lights, gLights, objects, cam, config);

    auto load_end = std::chrono::steady_clock::now();
    std::cout<<"Loading completed in: " << (load_end-load_start)/std::chrono::milliseconds(1)<< " (ms)" << std::endl;

    ImageArray img(cam.H_RES, cam.V_RES);
    cimg_library::CImg<float> display_image(cam.H_RES, cam.V_RES,1,3,0);
    cimg_library::CImgDisplay display(display_image, "Oke's Path Tracer!");
    //creating filename....
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream filename;
    filename << renderDest << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

    bool looping = true;

    //Creation of BoundVolume Hierarchy
    auto bvh_start = std::chrono::steady_clock::now();
    BoundVolume* scene_bv = BoundVolume::compute_bound_volume(objects);
    Vector center = Vector(0,0,0);
    for(unsigned int k = 0; k < objects.size(); k++)
    {
        center  = center + objects[k]->position;
    }

    center = center / objects.size();
    BoundVolumeHierarchy* bvh = new BoundVolumeHierarchy(scene_bv, center);

    for (auto obj: objects)
    {
        bvh->insert_object(obj,0);
    }
    auto top_node_bv = bvh->build_BVH();  // the obj this pointer points to self deletes.
    auto bvh_end = std::chrono::steady_clock::now();
    std::cout<<"BVH Constructed in: " << (bvh_end-bvh_start)/std::chrono::milliseconds(1)<< " (ms)" << std::endl;

    //Creation of samplers used for montecarlo integration.
    Sampler* sampler1 = new RandomSampler();//HaltonSampler(7, rand()%5000 + 1503);//
    Sampler* sampler2 = new RandomSampler();//HaltonSampler(3, rand()%5000 + 5000); //

    /////////////////////////////////////// CAST & DISPLAY  CODE /////////////////////////////
    auto cast_start = std::chrono::steady_clock::now();
    int s;
    double exponent = 1/4.0;
    for(s=0;s<config.spp; s++)
    {
        cast_rays_multithread(config, cam, img, sampler1, sampler2, bvh, objects, lights, gLights);
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
            looping = false;
            break;
        }
        img.floatArrayUpdate();
    }

    delete sampler1;
    delete sampler2;
    sampler1 = nullptr;
    sampler2 = nullptr;

    lights.clear();
    for (auto p : objects)
    {
        delete p;
        p = nullptr;
    }
    delete bvh;
    bvh = nullptr;
    objects.clear();
    auto cast_end = std::chrono::steady_clock::now();

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

    filename << "_spp-" << s <<"_cast-"<<(cast_end-cast_start)/std::chrono::seconds(1)<<".png";


    // Draw/Save code
    auto save_start = std::chrono::steady_clock::now();
    if(config.stretch == "norm")
    {
        img.normalise(img.MAX_VAL);
    }else if(config.stretch == "gamma")
    {
        img.gammaCorrection(1.0/2.2);
        img.normalise(img.MAX_VAL);
    }else if(config.stretch == "rein")
    {
        img.normalise(1.0);
        img.reinhardToneMap();
        img.normalise(img.MAX_VAL);
    }
    draw(img, filename.str());
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



void draw(ImageArray& img, std::string filename)
{
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
