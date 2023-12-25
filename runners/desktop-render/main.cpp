#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <math.h>
#include <string>
#include <pngpp-w.hpp>
#include "CImg-w.h"

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
#include "Sphere.h"
#include "GObject.h"
#include "Plane.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Markup-w.h"




#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using namespace std;


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
Hit intersect(const Vector& src, const Vector& ray_dir, BoundVolumeHierarchy* bvh);
Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2, BoundVolumeHierarchy* bvh, const Config& config, const vector<GObject*>& objects, const vector<Light>& lights);
void cast_rays(const Camera& cam, const ImageArray& img, int row_start, int row_end);
void deserialize(string filename, vector<Light>& lights, vector<GObject*>& gLights, vector<GObject*>& objects, Camera& cam, Config& config);
void cast_rays_multithread(const Config& config, const Camera& cam, const ImageArray& img, Sampler* sampler1, Sampler* sampler2, BoundVolumeHierarchy* bvh, const vector<GObject*>& objects, const vector<Light>& lights, const vector<GObject*>& gLights);
double double_rand(const double & min, const double & max);
Color trace_rays_iterative(const Vector& origin, const Vector& ray_dir, BoundVolumeHierarchy* bvh, const Config& config, int depth, Sampler* ha1, Sampler* ha2, const vector<GObject*>& objects, const vector<GObject*>& gLights);
Vector uniform_hemisphere(double u1, double u2, Vector& n);
Vector cosine_weighted_hemisphere(double u1, double u2, Vector& n);
void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3);
Vector uniform_sphere(double u1, double u2);
Mesh* obj_reader(string filename);
void clear_globals();
bool is_light(GObject* obj);

// global var declaration
uint64_t numPrimaryRays = 0;
uint64_t numRayTrianglesTests = 0;
uint64_t numRayTrianglesIsect = 0;

Vector snells_law(const Vector& incident_ray, const Vector& normal, double cos_angle, double n_1, double n_2)
//assume the two rays are normalised, thus dot product returns the cosine of them.
//takes cos_angle to avoid doing a redundant dot product.
{
    Vector ray_hor = incident_ray - cos_angle*normal;
    double sin_theta_2 = ray_hor.abs()*n_1/n_2;
    Vector refr_ray = normalise(-1*normal + normalise(ray_hor)*sin_theta_2);
    return refr_ray;
}

double schlick_fresnel(double cos_angle, double n_1, double n_2)
//returns probabibility of reflection based on angle relative to normal. (0 to 90 degrees)
{
    double R_0 = (n_1-n_2)/(n_1+ n_2);
    R_0 = R_0 * R_0;

    double R_theta = R_0 + (1 - R_0)*pow((1-cos_angle),5);
    return R_theta;
}

Vector uniform_sphere(double u1, double u2)
{
    const double r = sqrt(1.0 - u1*u1);
	const double phi = 2 * PI * u2;
	Vector ray = Vector(cos(phi)*r, sin(phi)*r, u1);

	return ray;
}
Vector uniform_hemisphere(double u1, double u2, Vector& n) {
	const double r = sqrt(1.0 - u1*u1);
	const double phi = 2 * PI * u2;
	Vector ray = Vector(cos(phi)*r, sin(phi)*r, u1);
	if (ray.dot(n)<0)
    {
        return -1*ray;
    }
	return ray;
}

Vector cosine_weighted_hemisphere(double u1, double u2, const Vector& n)
// taken from http://www.rorydriscoll.com/2009/01/07/better-sampling/
//modified to ensure on the correct hemisphere
{
    const double r = sqrt(u1);
    const double theta = 2 * PI * u2;

    const double x = r * cos(theta);
    const double y = r * sin(theta);
    const double z = 1 - x*x - y*y;

    Vector ray = Vector(x,y,z);
    if(ray.dot(n)<0)
    {
        return -1*ray;
    }
    return ray;
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

bool is_light(GObject* a)
{
    if(a->emission.r >0 || a->emission.g > 0 || a->emission.b > 0)
    {
        return true;
    }
    return false;
}

void clear_globals()
{

    numPrimaryRays = 0;
    numRayTrianglesTests = 0;
    numRayTrianglesIsect = 0;
}

int main()
{
    int window_width = 800;
    int window_height = 800;

    vector<GObject*> objects;
    vector<Light> lights;
    vector<GObject*> gLights; //GObjects that have emission.

    Camera cam;
    Config config;
    int orw = 20;

    //initial call to deserialize just so I can define ImageArray...
    cout<<"Loading scene from scene.xml..." << endl;
    auto load_start = chrono::steady_clock::now();
    std::string rootPath = RootPath;
    std::string renderDest = rootPath + "/renders/";

    deserialize(rootPath + "/data/scenes/scene.xml", lights, gLights, objects, cam, config);

    auto load_end = chrono::steady_clock::now();
    cout<<"Loading completed in: " << (load_end-load_start)/chrono::milliseconds(1)<< " (ms)" << endl;

    ImageArray img(cam.H_RES, cam.V_RES);
    cimg_library::CImg<float> display_image(cam.H_RES, cam.V_RES,1,3,0);
    cimg_library::CImgDisplay display(display_image, "Oke's Path Tracer!");
    //creating filename....
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    ostringstream filename;
    filename << renderDest << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

    bool looping = true;

    //Creation of BoundVolume Hierarchy
    auto bvh_start = chrono::steady_clock::now();
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
    auto bvh_end = chrono::steady_clock::now();
    cout<<"BVH Constructed in: " << (bvh_end-bvh_start)/chrono::milliseconds(1)<< " (ms)" << endl;

    //Creation of samplers used for montecarlo integration.
    Sampler* sampler1 = new RandomSampler();//HaltonSampler(7, rand()%5000 + 1503);//
    Sampler* sampler2 = new RandomSampler();//HaltonSampler(3, rand()%5000 + 5000); //

    /////////////////////////////////////// CAST & DISPLAY  CODE /////////////////////////////
    auto cast_start = chrono::steady_clock::now();
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
    auto cast_end = chrono::steady_clock::now();

    cout << "Casting completed in: "<< setw(orw) << (cast_end - cast_start)/chrono::milliseconds(1)<< " (ms)"<<endl;
    cout << "Number of primary rays: " << setw(orw+1) << numPrimaryRays << endl;
    cout << "Number of Triangle Tests: " << setw(orw) << numRayTrianglesTests << endl;
    cout << "Number of Triangle Intersections: " <<setw(orw-11) << numRayTrianglesIsect << endl;
    cout << "Percentage of sucesful triangle tests: " << setw(orw-12) << 100*(float) numRayTrianglesIsect/numRayTrianglesTests<< "%" << endl;
    cout << "----------------------------------------------------------\n\n\n\n"<<endl;
    cout << "Waiting for modification of scene.xml or close window to save" << endl;

    //Sample scaling, do not touch this as this ensures each image has same relative brightness regardless of no. samples.
    for (int i = 0; i < img.PIXEL_COUNT; ++i)
    {
        img.pixelMatrix[i] = img.pixelMatrix[i]/s;
    }

    filename << "_spp-" << s <<"_cast-"<<(cast_end-cast_start)/chrono::seconds(1)<<".png";


    // Draw/Save code
    auto save_start = chrono::steady_clock::now();
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
    auto save_end = chrono::steady_clock::now();
    cout << "Image Save completed in: "<< setw(orw-7) <<(save_end - save_start)/chrono::milliseconds(1)<< " (ms)"<<endl;
    cout << "----------------------------------------\n\n\n\n" << endl;


    //getch();
    return 0;

    /* Animation codes
    ostringstream filename;
    filename << "render" << setfill('0') <<setw(3)<< i << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";
      //system("D:\Programming\Raytracer\ffmpeg -f image2 -framerate 24 -i D:\Programming\Raytracer\renders\test%03d.png -pix_fmt yuv420p -b:v 0 -crf 30 -s 1000x1000 render2.webm");
    */
}

void cast_rays_multithread(const Config& config, const Camera& cam, const ImageArray& img, Sampler* sampler1, Sampler* sampler2, BoundVolumeHierarchy* bvh, const vector<GObject*>& objects, const vector<Light>& lights, const vector<GObject*>& gLights)
{
    int total_pixels = cam.V_RES*cam.H_RES;
    int cores_to_use = config.threads_to_use;//global
    volatile atomic<size_t> pixel_count(0);
    vector<future<void>> future_vector;

    for (int i = 0; i<cores_to_use; i++ )
    {
        future_vector.emplace_back(
            async(launch::async, [=,&cam, &img, &pixel_count]()
            {
                while(true)
                {
                    int x_index = pixel_count++;

                    if (x_index >=total_pixels)
                    {
                        break;
                    }

                    int y_index = x_index / cam.H_RES;
                    x_index = x_index%cam.H_RES;

                    //Box Muller Pixel Sampling
                    double x_offset;//= sampler1->next() - 0.5;
                    double y_offset;// = sampler2->next() - 0.5;

                    double u1 = sampler1->next()*0.9 + 0.1; //We scale the box muller distribution radial input to fit within a pixel
                    double u2 = sampler2->next();//*0.9 + 0.1;
                    double R  = sqrt(-2.0 * log(u1));
                    double angle = 2.0 * M_PI * u2;
                    x_offset = R * cos(angle);
                    y_offset = R * sin(angle);

                    Vector ray_dir = -cam.N*cam.n + cam.H*(((double)2*(x_index+x_offset)/(cam.H_RES-1)) -1)*cam.u + cam.V*(((double)2*(y_index+y_offset)/(cam.V_RES-1)) -1)*cam.v;
                    Vector ray_norm = normalise(ray_dir);
                    ray_dir = ray_norm*cam.focus_dist/(-1*ray_norm.dot(cam.n)); //this division ensures we get a planar focal plane, as opposed to spherical.
                    double aperture_radius = cam.aperture* sqrt(sampler1->next());
                    double aperture_angle = 2* M_PI * sampler2->next();
                    Vector aperture_u_offset = aperture_radius * cos(aperture_angle) * cam.u;
                    Vector aperture_v_offset = aperture_radius * sin(aperture_angle) * cam.v;
                    ray_dir = ray_dir -(aperture_u_offset + aperture_v_offset);

                    Color c = trace_rays_iterative(cam.pos+aperture_u_offset+aperture_v_offset, ray_dir, bvh, config, 0, sampler1, sampler2, objects, gLights);//shade(hit, 0, sampler1, sampler2, bvh, config, objects, lights);
                    //cout << img.pixelMatrix[0].r << endl;
                    img.pixelMatrix[img.index(x_index, y_index)] = (img.pixelMatrix[img.index(x_index, y_index)]) + c;
                }
            }));
    }
}

Hit intersect(const Vector& src, const Vector& ray_dir, BoundVolumeHierarchy* bvh)
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


Color trace_rays_iterative(const Vector& origin, const Vector& ray_dir, BoundVolumeHierarchy* bvh, const Config& config, int depth, Sampler* ha1, Sampler* ha2, const vector<GObject*>& objects, const vector<GObject*>& gLights)
{
    Vector o = origin; //copy
    Vector d = ray_dir; //copy
    Color c;
    Color weight = Color(1,1,1);
    bool ignore_direct = false;
    depth = 0;

    while (true)
    {
        if (depth> config.max_reflections)
        {
            break;
        }

        Hit hit = intersect(o, d, bvh); //see if current ray intersects something or not.

        if(hit.obj == nullptr || hit.t == -1)
        //nothing was hit;
        {
            break;
        }
        Vector hit_point = hit.src + hit.t*hit.ray_dir;
        double n_dot_ray = hit.n.dot(hit.ray_dir);
        if(is_light(hit.obj) && n_dot_ray < 0)
        {
            double divisor = 1;//max(1.0,hit.t*hit.t);

            if(!ignore_direct)
            {
                c = c + -1*weight*n_dot_ray*hit.obj->emission/(divisor *255.0);
            }else
            {
                c = c + -1*weight*n_dot_ray*hit.obj->emission/(divisor *255.0);
            }
            break;
        }

        //NEE
        /*
        if(hit.obj->brdf != 2)
        //not specular
        {
            auto light_i = int((gLights.size()-1) * ha2->next());
            auto light = gLights[light_i];
            auto prob_choosing_light = 1/gLights.size(); //uniform because idek how to IS this...
            Vector light_point = light->get_random_point(ha1->next(), ha2->next());
            Vector NEE_Ray = light_point - hit_point;
            Hit light_hit = intersect(hit_point, NEE_Ray, bvh);
            double NEE_Ray_length = NEE_Ray.abs();
            Vector out = normalise(NEE_Ray);
            double light_n_dot_NEE = light_hit.n.dot(out);
            if (NEE_Ray_length -0.0001 < light_hit.t < NEE_Ray_length +0.0001 && light_n_dot_NEE < 0)
            {
                Vector in = -1*d;

                auto length = NEE_Ray_length;
                Color brdf_coef = hit.color/255; //mat.eval(in, out)
                //c = c+ light->emission/255 * weight * brdf_coef  * hit.n.dot(out) * -1*light_n_dot_NEE  / ( length * length) / (prob_choosing_light);
            }
        }*/

        //auto survive_prob = 0.90;
       // if (ha1->next() > survive_prob) break;
        o = hit_point;
        if(hit.obj->brdf==2)
        {
            ignore_direct = false;
        }else
        {
            ignore_direct = true;
        }
        //ignore_direct = !mat.is_specular;
        Vector new_ray_dir;
        if(hit.obj->brdf==0)
        //diffuse
        {
            new_ray_dir = uniform_hemisphere(ha1->next(), ha2->next(), hit.n);
        }else if(hit.obj->brdf==2)
        //mirror
        {
           new_ray_dir = normalise(hit.ray_dir - hit.n * 2  *n_dot_ray);
        }else if(hit.obj->brdf==3)
        //constant density volume
        {
            double density =2;
            double scatter_prob = ha1->next() + 0.2;
            Hit volume_hit = intersect(hit_point+(d*0.0001), d, bvh); //we assume d is normalised
            if(hit.obj == volume_hit.obj && volume_hit.n.dot(d) >0 ) // we are inside the obj, not a tangent.
            {
                o = hit_point+(scatter_prob*density*d);
                if(volume_hit.t > scatter_prob*density) // We should scatter it.
                {

                    new_ray_dir = uniform_sphere(ha1->next(),ha2->next());
                }
                else
                {
                    new_ray_dir = d;
                }
            }
        }
        d= new_ray_dir;
        //o = hit_point;
        weight = weight* hit.color*hit.n.dot(new_ray_dir)/(255.0);
        if(weight.r <0)
        {
            weight= weight*-1; // to account for constant density volume doing back reflection
        }
        depth+=1;
    }


    return c;

    //old trace code

    /*
                Vector sample_dir = uniform_hemisphere(ha1->next(), ha2->next(), hit.n); ////can replace with halton series etc in the future.
                out_rays.push_back(normalise(sample_dir));

                //compute ray direct to light ray and in future to known points of light reflection (BDPT)
               /* for(unsigned int i = 0; i < objects.size(); i++)
                //in future may keep a separate vector for emissive objects (in cases of many objects)
                {
                    if(objects[i]->emission.r > 0 || objects[i]->emission.g >0 || objects[i]->emission.b > 0)
                    //if object is emissive
                    {
                        Vector light_point = objects[i]->get_random_point(ha1->next(), ha2->next());
                        Vector s =  normalise(light_point- hit_point);

                        //The below is only valid for opaque objects, ensure we have a valid reflection.
                        if(s.dot(hit.n) >0)
                        {
                            out_rays.push_back(s);
                        }
                    }
                }
        //mirror

                Vector next_ray = normalise(hit.ray_dir - hit.n * 2  *n_dot_ray); // n_dot_ray being the cos angle
                out_rays.push_back(next_ray);
                break;*/
}

Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2, BoundVolumeHierarchy* bvh, const Config& config, const vector<GObject*>& objects, const vector<Light>& lights)
{
    Color c = Color(0, 0, 0);

    if(hit.obj == nullptr || hit.t == -1)
    {
        return c;
    }

    if(hit.obj != nullptr && hit.obj->emission.r>0 && hit.n.dot(hit.ray_dir)< 0) //bad check to see if emissive. && hit.obj->emission.r >0 && hit.n.dot(hit.ray_dir)<0
    {
        return hit.obj->emission/255;
        //c = c+ (hit.obj->emission)/255;
    }

    if (reflection_count> config.max_reflections)
    {
        return c;
    }

    Vector p = hit.src + hit.t * hit.ray_dir; //hit point
    Vector n = hit.n; //hit.obj->normal(p);
    //Vector v = hit.src - p; //vector from point to viewer

    if(hit.obj->brdf == 0 || hit.obj->brdf == 1)
    //diffuse object
    {
        //indirect illumination
        Vector v1, v2;
        create_orthonormal_basis(n, v1, v2);

        Vector sample_dir = cosine_weighted_hemisphere(ha1->next(), ha2->next(), hit.n); ////can replace with halton series etc in the future.
        Vector transformed_dir;
        //I could use my matrix class here but this will save some time on construction/arithmetic maybe...
        transformed_dir.x = Vector(v1.x, v2.x, n.x).dot(sample_dir);
        transformed_dir.y = Vector(v1.y, v2.y, n.y).dot(sample_dir);
        transformed_dir.z = Vector(v1.z, v2.z, n.z).dot(sample_dir);
        double cos_t = transformed_dir.dot(n);
        Hit diffuse_relfec_hit = intersect(p, transformed_dir, bvh);
        if(diffuse_relfec_hit.t != -1 && diffuse_relfec_hit.n.dot(transformed_dir)< 0)//Inbound ray hits correct face (outbound normal vector)
        {
            Color diffuse_reflec_color = shade(diffuse_relfec_hit,reflection_count+1, ha1, ha2, bvh, config, objects, lights);
            double divisor =max(1.0,diffuse_relfec_hit.t*diffuse_relfec_hit.t);
            c = c + diffuse_reflec_color*hit.color/(divisor*255);//idk where 0.1 comes from.cos_t*
        }

        //direct illumination
        //for(unsigned int i = 0; i < lights.size(); i++)
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            if(objects[i]->emission.r > 0 || objects[i]->emission.g !=0 || objects[i]->emission.b != 0)
            {
                Vector light_point = objects[i]->get_random_point(ha2->next(), ha1->next());//lights[i].position;//
                Vector s =  light_point- p;

                //Inbound ray hits correct face (outbound normal vector)

                double dist = s.abs();
                s = normalise(s);

                Hit shadow = intersect(p, s, bvh); //0.001 offset to avoid collision withself //+0.001*s

                //old if statement back in day of point light source.if(shadow.obj == nullptr || shadow.obj == objects[i] || shadow.t < 0.0001 || shadow.t > dist-0.0001)//
                //the object is not occluded from the light.
                double cosine_term  = shadow.n.dot(s) *-1; //term used to simulate limb darkening?
                if(shadow.obj == objects[i] && cosine_term>0)
                {
                    if(s.dot(n)> 0 ) // light is on right side of the face of obj normal
                    {
                        //diffuse
                        double divisor = max(1.0, shadow.t*shadow.t);
                        c = c + hit.color *cosine_term* objects[i]->emission * s.dot(n)/(divisor * 255 * 255); // lights[i].color

                        if(hit.obj->brdf == 1)
                        //diffuse object with specular...
                        {
                            Vector h = normalise(s + normalise(-1 * hit.t * hit.ray_dir));
                            double val = h.dot(n)/h.abs();
                            c = c + cosine_term*objects[i]->emission* pow(val, hit.obj->shininess)/(divisor*255);

                        }
                    }
                }
            }
        }
    }else if (hit.obj->brdf == 2)
    //glass
    {
        double n_1 = 1;
        double n_2 = 1.6;
        double cos_angle = hit.n.dot(hit.ray_dir);
        Vector next_ray;
        double PR;
        double crit_angle = -1;
        if (cos_angle>0)
        //ray incident on the obj
        {
            std::swap(n_1, n_2);
            crit_angle = asin(n_1/n_2);
        }
        double angle = acos(abs(cos_angle));
        if(crit_angle != -1 && angle > crit_angle)
        {
             next_ray = normalise(hit.ray_dir - hit.n * 2  *cos_angle);
        }else
        {
             PR =schlick_fresnel(abs(cos_angle), n_1, n_2);


            if(ha1->next() < PR)
            //reflection
            {
                next_ray = normalise(hit.ray_dir - hit.n * 2  *cos_angle);
            }else
            //refraction
            {
                next_ray = snells_law(hit.ray_dir, hit.n, cos_angle, n_1, n_2);
            }
        }


        Hit trace_hit = intersect(p, next_ray, bvh);
        if(trace_hit.t != -1)
        {
            Color color = shade(trace_hit, reflection_count+1, ha1, ha2, bvh, config, objects, lights);
            c = c + 0.9*color*hit.color/255; //factor of 0.9 for attenuation
        }
        //copy paste for direct illumination...
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            if(objects[i]->emission.r > 0 || objects[i]->emission.g !=0 || objects[i]->emission.b != 0)
            {
                Vector light_point = objects[i]->get_random_point(ha2->next(), ha1->next());//lights[i].position;//
                Vector s =  light_point- p;
                double dist = s.abs();
                s = normalise(s);

                Hit shadow = intersect(p, s, bvh); //0.001 offset to avoid collision withself //+0.001*s

                if(shadow.obj == nullptr || shadow.obj == objects[i] || shadow.t < 0.0001 || shadow.t > dist-0.0001)//
                //the object is not occluded from the light.
                {
                    if(s.dot(n)>= 0 ) // light is on right side of the face of obj normal
                    {
                        //diffuse
                        //c = c + hit.color * objects[i]->emission * s.dot(n)/(255*255); // lights[i].color


                        //diffuse object with specular...
                        {
                            //Vector h = normalise(s + normalise(-1*hit.t * hit.ray_dir));
                            //double val = h.dot(n)/h.abs();
                            //c = c + objects[i]->emission* pow(val, hit.obj->shininess)/(255);

                        }
                    }
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
            Color c = img.pixelMatrix[img.index(x,y)];
            image[y][x] = png::rgb_pixel((int)c.r, (int)c.g,(int) c.b);
        }
    }

    image.write(filename);
}

void deserialize(string filename, vector<Light>& lights, vector<GObject*>& gLights, vector<GObject*>& objects, Camera& cam, Config& config)
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
