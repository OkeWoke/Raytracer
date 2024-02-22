#include "render.h"
#include <future>
#include "math.h"
//#define DEBUG_SINGLE_THREAD
#define DEBUG_BACK_OF_SURFACE

void cast_rays_multithread(const Scene& scene,
                           ImageArray& img,
                           const Sampler& sampler1,
                           const Sampler& sampler2,
                           const BoundVolumeHierarchy& bvh)
{
    int total_pixels = scene.cam.V_RES*scene.cam.H_RES;
    int cores_to_use = scene.config.threads_to_use;//global
    volatile std::atomic<size_t> pixel_count(0);
    std::vector<std::future<void>> future_vector;

    for (int i = 0; i<cores_to_use; i++ )
    {
#ifndef DEBUG_SINGLE_THREAD
        future_vector.emplace_back(
                async(std::launch::async, [=, &scene, &img, &pixel_count, &sampler1, &sampler2, &bvh]()
                {
#endif
                    while(true)
                    {
                        int x_index = pixel_count++;

                        if (x_index >=total_pixels)
                        {
                            break;
                        }

                        int y_index = x_index / scene.cam.H_RES;
                        x_index = x_index%scene.cam.H_RES;

                        //Box Muller Pixel Sampling. Random Radial offset from center of the pixel. (Helps with antialiasing)
                        double x_offset;//= sampler1->next() - 0.5;
                        double y_offset;// = sampler2->next() - 0.5;

                        double u1 = sampler1.next()*0.9 + 0.1; //We scale the box muller distribution radial input to fit within a pixel
                        double u2 = sampler2.next();//*0.9 + 0.1;
                        double R  = sqrt(-2.0 * log(u1));
                        double angle = 2.0 * M_PI * u2;
                        x_offset = R * cos(angle);
                        y_offset = R * sin(angle);

                        // Ray Construction
                        Vector ray_dir = -scene.cam.N*scene.cam.n +  // N Direction
                                          scene.cam.H*(((double)2*(x_index+x_offset)/(scene.cam.H_RES-1)) -1)*scene.cam.u + // U Direction
                                          scene.cam.V*(((double)2*(y_index+y_offset)/(scene.cam.V_RES-1)) -1)*scene.cam.v; // V Direction
                        Vector ray_norm = normalise(ray_dir);

                        // Aperture Focus PLane Modification to ray cast
                        ray_dir = ray_norm*scene.cam.focus_dist/(-1*ray_norm.dot(scene.cam.n)); //this division ensures we get a planar focal plane, as opposed to spherical.
                        double aperture_radius = scene.cam.aperture* sqrt(sampler1.next());
                        double aperture_angle = 2* M_PI * sampler2.next();
                        Vector aperture_u_offset = aperture_radius * cos(aperture_angle) * scene.cam.u;
                        Vector aperture_v_offset = aperture_radius * sin(aperture_angle) * scene.cam.v;
                        ray_dir = ray_dir -(aperture_u_offset + aperture_v_offset);
                        Vector ray_origin = scene.cam.pos + aperture_u_offset + aperture_v_offset;

                        // Perform trace into the scene!
                        Color c = trace_rays_iterative(ray_origin, ray_dir, bvh, scene.config, 0, sampler1, sampler2, scene.objects, scene.gLights);

                        // Add the color to the image pixel.
                        img.pixelMatrix[img.index(x_index, y_index)] = (img.pixelMatrix[img.index(x_index, y_index)]) + c;
                    }
#ifndef DEBUG_SINGLE_THREAD
        }));
#endif
    }
}

Color trace_rays_iterative(const Vector& origin,
                           const Vector& ray_dir,
                           const BoundVolumeHierarchy& bvh,
                           const Config& config,
                           int depth,
                           const Sampler& ha1,
                           const Sampler& ha2,
                           const std::vector<std::shared_ptr<GObject>>& objects,
                           const std::vector<std::shared_ptr<GObject>>& gLights)
{
    const double epsilon = 0.0001;
    Vector o = origin; //copy
    Vector d = normalise(ray_dir); //copy
    Color c;
    Color weight = Color(1,1,1);
    depth = 0;

    while (true)
    {
        if (depth > config.max_reflections)
        {
            break;
        }

        auto src = o + epsilon*d;
        GObject::intersection hit = bvh.intersect(src, d, 0); // epsilon offset to avoid collision with self

        if(hit.obj_ref == nullptr || hit.t == -1) // nothing was hit;
        {
            break;
        }

        if (hit.color.r == -1) // texture hit
        {
            hit.color = hit.obj_ref->color;
        }

        double n_dot_ray = hit.n.dot(d); // indicates we hit back of obj if positive.

#ifdef DEBUG_BACK_OF_SURFACE
        if (n_dot_ray > 0)
        {
            return Color(0,0,255); // blue return debug back of surface
        }
#endif
        if(hit.obj_ref->is_light()) // hit front of light
        {
            double divisor = 1;//max(1.0,hit.t*hit.t);
            c = c + -1*weight*n_dot_ray*hit.obj_ref->emission/(divisor *255.0);
            break;
        }

        o = src + hit.t * d; // new Origin point. Located at the hit or bounce point.

        switch(hit.obj_ref->brdf)
        {
            case GObject::BRDF::PHONG_DIFFUSE:
                d = Utility::uniform_hemisphere(ha1.next(), ha2.next(), hit.n); // should be normal
                weight = weight* hit.color*hit.n.dot(d)/(255.0);
                break;
            case GObject::BRDF::MIRROR:
                d = normalise(d - hit.n * 2  *n_dot_ray);
                weight = weight* hit.color*hit.n.dot(d)/(255.0);
                break;
            default:
                throw std::runtime_error("BRDF not supported");
        }

        depth+=1;
    }

    return c;
}
