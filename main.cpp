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

#include "Vector.h"
#include "imageArray.h"
#include "Sphere.h"
#include "GObject.h"
#include "Light.h"
#include "Plane.h"
#include "Camera.h"
#include "Markup.h"

using namespace std;

struct Hit
{
    Vector src;
    Vector ray_dir;
    double t;
    GObject* obj;
};

void draw(ImageArray img, string filename);
Hit intersect(Vector src, Vector ray_dir);
Color shade(const Hit& hit, int reflection_count);
void cast_rays(Camera& camera, ImageArray& img);
void deserialize(string filename);

vector<GObject*> objects;
vector<Light> lights;
Camera cam;

int main()
{
    //Initialisation
    int H_RES = 1000;
    int V_RES = 1000;
    double N = 0.2; //distance between eye and screen
    double H = 0.5;
    double V = 0.5;
    Vector eye = Vector(0,40,450);//eye is at the center.
    Vector u = Vector(1, 0, 0);//horizontal perp;
    Vector v = Vector(0, 1, 0);//vertical perp;
    Vector n = Vector(0, 0, -1);// Looking into Z dir
    cam = Camera(H_RES,V_RES,N,H,V,u,v,n,eye);

    cout<<"Loading scene from xml..." << endl;
    deserialize("scene.xml");
    cout<<"Loading complete" << endl;

    ImageArray img(cam.H_RES, cam.V_RES);

    cout << "Starting initial ray cast..." << endl;
    auto start = chrono::steady_clock::now();
    cast_rays(cam, img);
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    ostringstream filename;
    filename << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";
    draw(img, filename.str());
    img.clearArray();

    /* Animation codes
    ostringstream filename;
    filename << "render" << setfill('0') <<setw(3)<< i << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";
    */

    cout << "Finished!" << endl;
    auto end = chrono::steady_clock::now();
    cout << "Completed in: "<<chrono::duration_cast<chrono::seconds>(end - start).count()<< " (s)"<<endl;
    //system("D:\Programming\Raytracer\ffmpeg -f image2 -framerate 24 -i D:\Programming\Raytracer\renders\test%03d.png -pix_fmt yuv420p -b:v 0 -crf 30 -s 1000x1000 render2.webm");
    img.deleteArray();
    getch();
    return 0;
}

void cast_rays(Camera& cam, ImageArray& img)
{
    for(int x = 0; x < cam.H_RES; x++)
    {
        for(int y = 0; y < cam.V_RES; y++)
        {
            Vector ray_dir = -cam.N*cam.n + cam.H*(((double)2*x/(cam.H_RES-1)) -1)*cam.u + cam.V*(((double)2*y/(cam.V_RES-1)) -1)*cam.v;
            Hit hit = intersect(cam.pos, ray_dir);
            Color c = shade(hit, 0);
            img.pixelMatrix[x][y] = c;
        }
    }
}

Hit intersect(Vector src, Vector ray_dir)
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
    int max_reflections = 4;
    int min_reflectivity = 0.3;

    if(hit.obj == NULL)
    {
        return Color(0,0,0);
    }

    Vector p = hit.src + hit.t*hit.ray_dir; //hit point
    Vector n = hit.obj->normal(p);
    Vector v = hit.src - p; //vector from point to viewer
    Color c = Color(0, 0, 0);

    for(unsigned int i = 0; i < lights.size(); i++)
    {
        //ambient
        c = c + (hit.obj->ambient * lights[i].ambient);

        Vector s = lights[i].position - p;
        Vector h = normalise(normalise(s) + normalise(v));
        Hit shadow = intersect(p,s);
        if(shadow.obj == NULL || shadow.t <0 || shadow.t >1)
        {
            if(s.dot(n)> 0 )
            {
                //diffuse
                c = c + hit.obj->diffuse * lights[i].diffuse * ((normalise(s).dot(n)));//(s.abs()*s.abs());

                //specular
                double val = h.dot(n)/ (h.abs() * n.abs());
                c = c + hit.obj->specular * lights[i].specular * pow(val, hit.obj->shininess);//(s.abs()*s.abs());
            }
        }

        //we can have reflections even if the object is visible but in shadow.
        if(reflection_count < max_reflections && hit.obj->reflectivity>=min_reflectivity)
        {
            Hit reflection = intersect(p, hit.ray_dir-n*2*hit.ray_dir.dot(n));
            Color reflec_color = shade(reflection,reflection_count+1);

            c = c +hit.obj->reflectivity * reflec_color;
        }
    }

    return c;
}

void draw(ImageArray img, string filename)
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
{
    CMarkup xml;
    xml.Load(filename);
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
        else if(element=="Camera")
        {
            Camera::deserialize(xml.GetSubDoc(),cam);
        }
        else if(element=="Light")
        {
            Light l = Light();
            l.deserialize(xml.GetSubDoc());
            lights.push_back(l);
        }
    }
}
