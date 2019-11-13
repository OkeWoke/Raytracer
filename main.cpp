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

#include "Vector.h"
#include "imageArray.h"
#include "Sphere.h"
#include "GObject.h"
#include "Light.h"
#include "Plane.h"
#include "Camera.h"
#include "Markup.h"

#include "CImg.h"

using namespace std;
using namespace cimg_library;
struct Hit
{
    Vector src;
    Vector ray_dir;
    double t;
    GObject* obj;
};

struct Config
{
    int threads_to_use;
    int max_reflections;
};

void draw(ImageArray& img, string filename);
Hit intersect(const Vector& src, const Vector& ray_dir);
Color shade(const Hit& hit, int reflection_count);
void cast_rays(const Camera& cam, const ImageArray& img, int row_start, int row_end);
void deserialize(string filename);
void cast_rays_multithread(const Camera& cam, const ImageArray& img);

vector<GObject*> objects;
vector<Light> lights;
Camera cam;
Config config;

int main()
{
    //Initialisation
    cout<<"Loading scene from scene.xml..." << endl;
    deserialize("scene.xml");
    cout<<"Loading complete" << endl;

    ImageArray img(cam.H_RES, cam.V_RES);

    cout << "Starting initial ray cast on "<< config.threads_to_use << " threads..."<< endl;
    auto start = chrono::steady_clock::now();
    cast_rays_multithread(cam,img);
    auto ray_end = chrono::steady_clock::now();
    cout << "Casting completed in: "<<(ray_end - start)/chrono::milliseconds(1)<< " (ms)"<<endl;


    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    ostringstream filename;
    filename << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";


    CImg<float> image(cam.H_RES, cam.V_RES,1,3,0);
    CImgDisplay display(image, "Raytracer!");
    while (!display.is_closed())
    {
        auto start = chrono::steady_clock::now();
        //display.wait();

        for (int y = 0; y < img.HEIGHT; ++y)
        {
            for (int x = 0; x < img.WIDTH; ++x)
            {
                image(x,y,0) = img.pixelMatrix[x][y].r;
                image(x,y,1) = img.pixelMatrix[x][y].g;
                image(x,y,2) = img.pixelMatrix[x][y].b;
            }
        }

        auto ray_end = chrono::steady_clock::now();
        cout << "Casting completed in: "<<(ray_end - start)/chrono::milliseconds(1)<< " (ms)"<<endl;
        image.display(display);
        lights.clear();
        objects.clear();
        deserialize("scene.xml");
        img.clearArray();
        cast_rays_multithread(cam, img);
    }

    cast_rays_multithread(cam, img);
    draw(img, filename.str());
    img.clearArray();
    cout << "Finished!" << endl;
    auto end = chrono::steady_clock::now();
    cout << "Completed in: "<<(end - ray_end )/chrono::milliseconds(1)<< " (ms)"<<endl;
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
    int rows_per_thread = cam.V_RES/config.threads_to_use;
    int remainder = cam.V_RES%config.threads_to_use;
    thread thread_array[config.threads_to_use] ;

    for(int i=0;i<config.threads_to_use-1;i++){
        thread_array[i] = thread(cast_rays, ref(cam), ref(img), i*rows_per_thread,(i+1)*rows_per_thread);
    }
    thread_array[config.threads_to_use-1] = thread(cast_rays, ref(cam), ref(img), (int)rows_per_thread*(config.threads_to_use-1), (int)(rows_per_thread*config.threads_to_use)+remainder);

    for(int i=0;i<config.threads_to_use;i++){
        thread_array[i].join();
    }
}

void cast_rays(const Camera& cam, const ImageArray& img, int row_start, int row_end)
{
    for(int x = 0; x < cam.H_RES; x++)
    {
        for(int y = row_start; y < row_end; y++)
        {
            Vector ray_dir = -cam.N*cam.n + cam.H*(((double)2*x/(cam.H_RES-1)) -1)*cam.u + cam.V*(((double)2*y/(cam.V_RES-1)) -1)*cam.v;
            Hit hit = intersect(cam.pos, ray_dir);
            Color c = shade(hit, 0);
            img.pixelMatrix[x][y] = c;
        }
    }
}

Hit intersect(const Vector& src, const Vector& ray_dir)
//Takes a source point and ray direction, checks if it intersects any object
//returns hit struct which contains 'meta data' about the interection.
{
    Hit hit;
    hit.src = src;
    hit.ray_dir= ray_dir;
    hit.t=-1;
    hit.obj = NULL;

    for(unsigned int i = 0; i < objects.size(); i++)
    {
        double t = objects[i]->intersect(src,ray_dir);

        if(t>0.0001 && (hit.obj==NULL || t<hit.t))//if hit is viisible and new hit is closer than previous
        {
            hit.t = t;
            hit.obj = objects[i];
        }
    }

    return hit;
}

Color shade(const Hit& hit, int reflection_count)
{

    int min_reflectivity = 0.3;

    if(hit.obj == NULL)
    {
        return Color(0,0,0);
    }

    Vector p = hit.src + hit.t * hit.ray_dir; //hit point
    Vector n = hit.obj->normal(p);
    Vector v = hit.src - p; //vector from point to viewer
    Color c = Color(0, 0, 0);

    for(unsigned int i = 0; i < lights.size(); i++)
    {
        Vector s = lights[i].position - p;
        Vector h = normalise(normalise(s) + normalise(v));
        double div_factor = sqrt(0.1*s.abs());

        //ambient
        c = c + (hit.obj->ambient * lights[i].ambient);//div;

        Hit shadow = intersect(p,s);
        if(shadow.obj == NULL || shadow.t < 0 || shadow.t > 1)
        {
            if(s.dot(n)> 0 )
            {
                //diffuse
                c = c + hit.obj->diffuse * lights[i].diffuse * ((normalise(s).dot(n))) / div_factor;

                //specular
                double val = h.dot(n) / h.abs();
                c = c + hit.obj->specular * lights[i].specular * pow(val, hit.obj->shininess) / div_factor;
            }
        }

        //we can have reflections even if the object is visible but in shadow.
        if(reflection_count < config.max_reflections && hit.obj->reflectivity>=min_reflectivity)
        {
            Hit reflection = intersect(p, hit.ray_dir - n * 2  *hit.ray_dir.dot(n));
            Color reflec_color = shade(reflection, reflection_count+1);

            c = c + hit.obj->reflectivity * reflec_color;
        }
    }

    return c;
}

void draw(ImageArray& img, string filename)
{
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
    xml.FindElem();
    config.threads_to_use = stoi(xml.GetAttrib("threads"));
    if (config.threads_to_use < 1){config.threads_to_use=1;}
    config.max_reflections = stoi(xml.GetAttrib("max_reflections"));

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
        else if(element == "Camera")
        {
            Camera::deserialize(xml.GetSubDoc(),cam);
        }
        else if(element == "Light")
        {
            Light l = Light();
            l.deserialize(xml.GetSubDoc());
            lights.push_back(l);
        }
    }
}
