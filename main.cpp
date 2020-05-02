#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <math.h>
#include <string>
#include <png.hpp>
#include <conio.h>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>
#include <functional>
#include <fstream>
#include <future>


#include "Camera.h"
#include "Light.h"
#include "Utility.h"
#include "Vector.h"
#include "Matrix.h"
#include "imageArray.h"
#include "BoundVolume.h"
#include "BoundVolumeHierarchy.h"

#include "GObjects/Sphere.h"
#include "GObjects/GObject.h"
#include "GObjects/Plane.h"
#include "GObjects/Triangle.h"
#include "GObjects/Mesh.h"

#include "ext/Markup.h"
#include "ext/CImg.h"

using namespace std;
using namespace cimg_library;
struct Hit
{
    Vector src;
    Vector ray_dir;
    Vector n;
    double t;
    GObject* obj;
    Color color;


    ~Hit()
    {
        obj = nullptr;
    }
};

struct Config
{
    int threads_to_use;
    int max_reflections;
    string stretch;
};

void draw(ImageArray& img, string filename);
Hit intersect(const Vector& src, const Vector& ray_dir);
Color shade(const Hit& hit, int reflection_count);
void cast_rays(const Camera& cam, const ImageArray& img, int row_start, int row_end);
void deserialize(string filename);
void cast_rays_multithread(const Camera& cam, const ImageArray& img);
void cast_rays_multithread_2(const Camera& cam, const ImageArray& img);
Mesh* obj_reader(string filename);

vector<GObject*> objects;
vector<Light> lights;
Camera cam;
Config config;

BoundVolumeHierarchy* bvh;
uint64_t numPrimaryRays = 0;
uint64_t numRayTrianglesTests = 0;
uint64_t numRayTrianglesIsect = 0;

int main()
{
    //Initialisation
    cout<<"Loading scene from scene.xml..." << endl;
    deserialize("scene.xml");
    cout<<"Loading complete" << endl;

    ImageArray img(cam.H_RES, cam.V_RES);

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    ostringstream filename;
    filename << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";

    CImg<float> image(cam.H_RES, cam.V_RES,1,3,0);
    CImgDisplay display(image, "Raytracer!");

    int orw = 20;
    lights.clear();
    for (auto p : objects)
    {
        delete p;
    }

    objects.clear();
    img.clearArray();

    while (!display.is_closed())
    {

        numPrimaryRays = 0;
        numRayTrianglesTests = 0;
        numRayTrianglesIsect = 0;

        deserialize("scene.xml");
        BoundVolume* scene_bv = BoundVolume::compute_bound_volume(objects);
        Vector center = Vector(0,0,0);
        for(unsigned int k = 0; k < objects.size(); k++)
        {
            center  = center + objects[k]->position;
        }
        center = center / objects.size();
        bvh = new BoundVolumeHierarchy(scene_bv, center);
        for (auto obj: objects)
        {
            bvh->insert_object(obj,0);
        }
        auto aaa = bvh->build_BVH();
        auto start = chrono::steady_clock::now();
        cast_rays_multithread(cam, img);
        auto ray_end = chrono::steady_clock::now();

        cout << "Casting completed in: "<< setw(orw) << (ray_end - start)/chrono::milliseconds(1)<< " (ms)"<<endl;
        cout << "Number of primary rays: " << setw(orw+1) << numPrimaryRays << endl;
        cout << "Number of Triangle Tests: " << setw(orw) << numRayTrianglesTests << endl;
        cout << "Number of Triangle Intersections: " <<setw(orw-11) << numRayTrianglesIsect << endl;
        cout << "Percentage of sucesful triangle tests: " << setw(orw-12) << 100*(float) numRayTrianglesIsect/numRayTrianglesTests<< "%" << endl;

        //display.wait();
        start = chrono::steady_clock::now();
        if(config.stretch == "norm")
        {
            img.normalise();
        }else if(config.stretch == "gamma")
        {
            img.gammaCorrection();
            img.normalise();
        }

        for (int y = 0; y < img.HEIGHT; ++y)
        {
            for (int x = 0; x < img.WIDTH; ++x)
            {
                image(x,y,0) = img.pixelMatrix[x][y].r;
                image(x,y,1) = img.pixelMatrix[x][y].g;
                image(x,y,2) = img.pixelMatrix[x][y].b;
            }
        }

        image.display(display);

        auto render_end = chrono::steady_clock::now();
        cout << "Image display completed in: "<< setw(orw-7) <<(render_end - start)/chrono::milliseconds(1)<< " (ms)"<<endl;
        cout << "----------------------------------------\n\n\n\n" << endl;
        lights.clear();
        for (auto p : objects)
        {

            delete p;
        }
        delete bvh;


        objects.clear();
        img.clearArray();
    }

    objects.clear();
    deserialize("scene.xml");

    img.clearArray();

    cast_rays_multithread(cam, img);
    for (auto p : objects)
    {

        delete p;
    }

    objects.clear();
    draw(img, filename.str());
    img.clearArray();
    cout << "Finished!" << endl;

    img.deleteArray();
    getch();
    return 0;

    /* Animation codes
    ostringstream filename;
    filename << "render" << setfill('0') <<setw(3)<< i << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";
      //system("D:\Programming\Raytracer\ffmpeg -f image2 -framerate 24 -i D:\Programming\Raytracer\renders\test%03d.png -pix_fmt yuv420p -b:v 0 -crf 30 -s 1000x1000 render2.webm");
    */
}

void cast_rays_multithread(const Camera& cam, const ImageArray& img)
{
    int total_pixels = cam.V_RES*cam.H_RES;
    int cores_to_use = config.threads_to_use;//global
    //uint64_t pixel_count = 0;
    volatile atomic<size_t> pixel_count(0);
    vector<future<void>> future_vector;
    for (int i = 0; i<cores_to_use; i++ )
    {

        future_vector.emplace_back(
            async(launch::async, [=,&cam, &img, &pixel_count]()
            {
                while(true)
                {
                    int x_index = pixel_count++;//pixel_count;
                    //__sync_fetch_and_add(&pixel_count, 1);//not sure if this will lead to non atomic behaviour with above line...

                    if (x_index >=total_pixels)
                    {
                        break;
                    }


                    int y_index = x_index / cam.H_RES;
                    x_index = x_index%cam.H_RES;
                    Vector ray_dir = -cam.N*cam.n + cam.H*(((double)2*x_index/(cam.H_RES-1)) -1)*cam.u + cam.V*(((double)2*y_index/(cam.V_RES-1)) -1)*cam.v;
                    Hit hit = intersect(cam.pos, ray_dir);
                    Color c = shade(hit, 0);
                    img.pixelMatrix[x_index][y_index] = c;
                }
            }));
    }
}

Hit intersect(const Vector& src, const Vector& ray_dir)
//Takes a source point and ray direction, checks if it intersects any object
//returns hit struct which contains 'meta data' about the interection.
{
    Hit hit;
    hit.src = src;
    hit.ray_dir= normalise(ray_dir);
    hit.t=-1;
    hit.obj = nullptr;

    GObject::intersection inter = bvh->intersect(src+0.0001*hit.ray_dir, hit.ray_dir, 0);
    if(inter.t > 0.0001 && (hit.obj == nullptr || (inter.t) < hit.t))//if hit is viisible and new hit is closer than previous
    {
        //yes the below is pretty shit, why do two so simillar structs exist....
        hit.t = inter.t;// ray_dir.abs();
        hit.obj = inter.obj_ref;

        if (inter.color.r == -1)
        {
            hit.color = inter.obj_ref->color;
        }else
        {
            hit.color = inter.color;
        }
        hit.n = inter.n;

    }

    return hit;
}

Color shade(const Hit& hit, int reflection_count)
{

    int min_reflectivity = 0.3;
    Color c = Color(0, 0, 0);
    if(hit.obj == nullptr || hit.t == -1)
    {
        return c;
    }
    //return Color(255,0,0);
    Vector p = hit.src + hit.t * hit.ray_dir; //hit point
    Vector n = hit.n; //hit.obj->normal(p);
    Vector v = hit.src - p; //vector from point to viewer

    for(unsigned int i = 0; i < lights.size(); i++)
    {
        Vector s = lights[i].position - p;
        s = normalise(s);
        Vector h = normalise(s + normalise(v));
        //ambient
        //c = c + (hit.color * lights[i].color)/(255);//div;

        Hit shadow = intersect(p, s); //0.001 offset to avoid collision withself //+0.001*s

        if(shadow.obj == nullptr|| shadow.t < 0 || shadow.t > 1)//)
        {
            //if(s.dot(n)> 0 ) // light is on right side of the face of obj normal
            {
                double div_factor = shadow.t*shadow.t*255;
                //diffuse
                c = c + hit.color * lights[i].color * s.dot(n) /(div_factor*255);

                //specular
                double val = h.dot(n) / h.abs();
                c = c +  lights[i].color * pow(val, hit.obj->shininess) /div_factor;
            }
        }

        //we can have reflections even if the object is visible but in shadow.
       if(reflection_count < config.max_reflections)//&& hit.obj->reflectivity>=min_reflectivity)
        {
            Vector reflec_ray = normalise(hit.ray_dir - n * 2  *hit.ray_dir.dot(n));
            Hit reflection = intersect(p+0.001*reflec_ray, reflec_ray);//0.001 offset to avoid collision withself
             Color reflec_color;
            if(reflection.t!= -1)
            {
                 reflec_color = shade(reflection, reflection_count+1);
            }

            c = c + hit.obj->reflectivity * reflec_color;
        }
    }

    return c;
}

void draw(ImageArray& img, string filename)
{
    img.gammaCorrection();
    img.normalise();
    png::image< png::rgb_pixel > image(img.WIDTH, img.HEIGHT);

    for (int y = 0; y < img.HEIGHT; ++y)
    {
        for (int x = 0; x < img.WIDTH; ++x)
        {
            Color c = img.pixelMatrix[x][y];
            image[y][x] = png::rgb_pixel((int)c.r, (int)c.g,(int) c.b);
        }
    }

    image.write("renders/"+filename);
}

void deserialize(string filename)
//Deserialises the scene/config.xml, modifies global structures and vectors
{
    CMarkup xml;
    xml.Load(filename);

    xml.FindElem(); //config
    config.threads_to_use = stoi(xml.GetAttrib("threads"));
    if (config.threads_to_use < 1){config.threads_to_use=1;}
    config.max_reflections = stoi(xml.GetAttrib("max_reflections"));
    config.stretch = xml.GetAttrib("stretch");

    xml.FindElem(); //camera
    Camera::deserialize(xml.GetSubDoc(),cam);

    while(xml.FindElem())
    {
        string element = xml.GetTagName();
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
        }
        else if(element == "Mesh")
        {

            Mesh* m = new Mesh();
            m->deserialize(xml.GetSubDoc());
            objects.push_back(m);
        }
    }
}
