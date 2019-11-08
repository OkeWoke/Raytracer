#include <iostream>
#include "Vector.h"
#include "imageArray.h"
#include "conio.h"
#include <png.hpp>
#include "Sphere.h"
#include "GObject.h"
#include <math.h>
#include "Light.h"
#include <string>
#include "Plane.h"
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
void cast_rays(int H_RES, int V_RES, double N, double H, double V, Vector eye, Vector u, Vector v, Vector n, ImageArray& img);
vector<GObject*> objects;
vector<Light> lights;

int main()
{
    //Initialisation
    cout << "Starting..." << endl;

    int H_RES = 1000;
    int V_RES = 1000;
    double N = 3; //distance between eye and screen
    double H = 0.5;
    double V = 0.5;

    Vector eye = Vector(0,40,-100);//eye is at the center.

    Vector u = Vector(1, 0, 0);//horizontal perp;
    Vector v = Vector(0, 1, 0);//vertical perp;
    Vector n = Vector(0, 0, -1);// Looking into Z dir

    ImageArray img(H_RES, V_RES);

    //Object and light creation
    Sphere sp1(Vector(0, -20, 500), 20, Color(0,128,0), Color(0,255,0), Color(255,255,255), 200, 1);
    Sphere sp2(Vector(0, 25, 550), 20, Color(128,0,128), Color(128,0,128), Color(255,255,255), 200, 1);
    Plane p1(Vector(0,-50,0), Vector(0,1,0), 0, 0, Color(128,0,0), Color(255,0,0), Color(255,255,255),200, 0.4);
    Light l1(Vector(0, 0, 450), Color(1,1,1),Color(1,1,1),Color(1,1,1));

    objects.push_back(&sp1);
    objects.push_back(&sp2);
    objects.push_back(&p1);
    lights.push_back(l1);
    for (unsigned int i = 0; i<100; i++)
    {
        cast_rays(H_RES, V_RES, N, H, V, eye, u, v, n , img);
        //save image
        draw(img, "test"+to_string(i)+".png");
        img.clearArray();
        sp1.position.y+=1;
    }
    //casting initial rays

    cout << "Finished!" << endl;
    getch();
    return 0;
}

void cast_rays(int H_RES, int V_RES, double N, double H, double V, Vector eye, Vector u, Vector v, Vector n, ImageArray& img)
{
    for(int x = 0; x < H_RES; x++)
    {
        for(int y = 0; y < V_RES; y++)
        {
            Vector ray_dir = -N*n + H*(((double)2*x/(H_RES-1)) -1)*u + V*(((double)2*y/(V_RES-1)) -1)*v;
            Hit hit = intersect(eye, ray_dir);
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
    int max_reflections = 5;
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
                c = c + hit.obj->diffuse * lights[i].diffuse * ((normalise(s).dot(n)));

                //specular

                double val = h.dot(n)/ (h.abs() * n.abs());
                c = c + hit.obj->specular * lights[i].specular * pow(val, hit.obj->shininess);
            }
            if(reflection_count < max_reflections && hit.obj->reflectivity>=min_reflectivity)
            {
                Hit reflection = intersect(p, hit.ray_dir-n*2*hit.ray_dir.dot(n));
                Color reflec_color = shade(reflection,reflection_count+1);

                c = c +hit.obj->reflectivity * reflec_color;
            }
        }


    }

    return c;
}

void draw(ImageArray img, string filename)
{
    //img.normalise();
    png::image< png::rgb_pixel > image(img.WIDTH, img.HEIGHT);

    for (int y = 0; y < img.HEIGHT; ++y)
    {
        for (int x = 0; x < img.WIDTH; ++x)
        {
            Color c = 255*img.pixelMatrix[x][y]/700;
            image[y][x] = png::rgb_pixel((int)c.r, (int)c.g,(int) c.b);
        }
    }

    image.write("renders/"+filename);
}

