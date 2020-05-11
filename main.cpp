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
#include <stdlib.h>
#include <time.h>

#include "Camera.h"
#include "Light.h"
#include "Utility.h"
#include "Vector.h"
#include "Matrix.h"
#include "imageArray.h"
#include "BoundVolume.h"
#include "BoundVolumeHierarchy.h"
#include "HaltonSampler.h"
#include "RandomSampler.h"
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
    int spp;
    string stretch;
};

// function prototypes
void draw(ImageArray& img, string filename);
Hit intersect(const Vector& src, const Vector& ray_dir);
Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2);
void cast_rays(const Camera& cam, const ImageArray& img, int row_start, int row_end);
void deserialize(string filename);
void cast_rays_multithread(const Camera& cam, const ImageArray& img);
void cast_rays_multithread_2(const Camera& cam, const ImageArray& img);
double double_rand(const double & min, const double & max);
Vector uniform_hemisphere(double u1, double u2);
void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3);
Mesh* obj_reader(string filename);

//var/obj declaration
vector<GObject*> objects;
vector<Light> lights;
Camera cam;
Config config;

BoundVolumeHierarchy* bvh;
uint64_t numPrimaryRays = 0;
uint64_t numRayTrianglesTests = 0;
uint64_t numRayTrianglesIsect = 0;


Vector uniform_hemisphere(double u1, double u2) {
	const double r = sqrt(1.0 - u1*u1);
	const double phi = 2 * PI * u2;
	return Vector(cos(phi)*r, sin(phi)*r, u1);
}

Vector cosine_weighted_hemisphere(double u1, double u2)
// taken from http://www.rorydriscoll.com/2009/01/07/better-sampling/
{
    const float r = sqrt(u1);
    const float theta = 2 * PI * u2;

    const float x = r * cos(theta);
    const float y = r * sin(theta);

    return Vector(x, y, sqrt(max((double)0, 1 - u1)));
}

//below function is taken from smallpaint
// given v1, set v2 and v3 so they form an orthonormal system
// (we assume v1 is already normalized)
void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3) {
	if (std::abs(v1.x) > std::abs(v1.y)) {
		// project to the y = 0 plane and construct a normalized orthogonal vector in this plane
		double invLen = 1.f / sqrtf(v1.x * v1.x + v1.z * v1.z);
		v2 = Vector(-v1.z * invLen, 0.0f, v1.x * invLen);
	} else {
		// project to the x = 0 plane and construct a normalized orthogonal vector in this plane
		double invLen = 1.0f / sqrtf(v1.y * v1.y + v1.z * v1.z);
		v2 = Vector(0.0f, v1.z * invLen, -v1.y * invLen);
	}
	v3 = v1 % v2;
}


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
    for (auto p : objects) // we only need this here and not in the main loop because the bvh destructor deletes the objects
    {
        delete p;
        p = nullptr;
    }

    objects.clear();
    while (!display.is_closed())
    {
        img.clearArray();
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
        auto top_node_bv = bvh->build_BVH();  // the obj this pointer points to self deletes.
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

        delete bvh;
        bvh = nullptr;


        objects.clear();
        if (display.is_closed())
        {
            draw(img, filename.str());
            cout << "Finished!" << endl;
            img.deleteArray();
        }
    }
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
                    Color c;


                    Sampler* ha1 = new RandomSampler();//HaltonSampler(7, rand()%5000 + 1503);
                    Sampler* ha2 = new RandomSampler();//HaltonSampler(3, rand()%5000 + 5000);

                    for(int s=0;s<config.spp; s++)
                    {
                        double x_offset = ha1->next() - 0.5;
                        double y_offset = ha2->next() - 0.5;
                        Vector ray_dir = -cam.N*cam.n + cam.H*(((double)2*(x_index+x_offset)/(cam.H_RES-1)) -1)*cam.u + cam.V*(((double)2*(y_index+y_offset)/(cam.V_RES-1)) -1)*cam.v;
                        Hit hit = intersect(cam.pos, ray_dir);
                        c =  c + shade(hit, 0, ha1, ha2);

                    }
                    delete ha1, ha2;
                    ha1 = nullptr;
                    ha2 = nullptr;

                    img.pixelMatrix[x_index][y_index] =  c/config.spp;
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

Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2)
{

    Color c = Color(0, 0, 0);
    //

    if(hit.obj == nullptr || hit.t == -1)
    {
        return c;
    }

    if(hit.obj != nullptr)
    {
        c = c+ hit.obj->emission;
    }

    if (reflection_count> config.max_reflections)
    {
        return c;
    }

    Vector p = hit.src + hit.t * hit.ray_dir; //hit point
    Vector n = hit.n; //hit.obj->normal(p);
    //Vector v = hit.src - p; //vector from point to viewer


    if(hit.obj->brdf == 0)
    //diffuse object
    {
        //indirect illumination
        Vector v1, v2;
        create_orthonormal_basis(n, v1, v2);

        Vector sample_dir = cosine_weighted_hemisphere(ha1->next(), ha2->next()); ////can replace with halton series etc in the future.
        Vector transformed_dir;
        //I could use my matrix class here but this will save some time on construction/arithmetic maybe...
        transformed_dir.x = Vector(v1.x, v2.x, n.x).dot(sample_dir);
        transformed_dir.y = Vector(v1.y, v2.y, n.y).dot(sample_dir);
        transformed_dir.z = Vector(v1.z, v2.z, n.z).dot(sample_dir);
        //double cos_t = transformed_dir.dot(n);
        Hit diffuse_relfec_hit = intersect(p, transformed_dir);
        if(diffuse_relfec_hit.t != -1)
        {
            Color diffuse_reflec_color = shade(diffuse_relfec_hit,reflection_count+1, ha1, ha2);
            c = c + diffuse_reflec_color*hit.color/255;//idk where 0.1 comes from.cos_t*
        }


        //direct illumination
        for(unsigned int i = 0; i < lights.size(); i++)
        {
            Vector s = lights[i].position - p;
            double dist = s.abs();
            s = normalise(s);

            Hit shadow = intersect(p, s); //0.001 offset to avoid collision withself //+0.001*s

            if(shadow.obj == nullptr|| shadow.t < 0.0001 || shadow.t > dist-0.0001)
            //the object is not occluded from the light.
            {
                if(s.dot(n)>= 0 ) // light is on right side of the face of obj normal
                {
                    double div_factor = 255; //shadow.t*shadow.t*
                    //diffuse
                    c = c + hit.color * lights[i].color * s.dot(n) /(div_factor*255);

                }
            }
        }
    }

    return c;
}

void draw(ImageArray& img, string filename)
{
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
    config.spp = stoi(xml.GetAttrib("samplesPP"));
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
