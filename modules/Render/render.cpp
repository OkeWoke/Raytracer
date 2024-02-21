#include "render.h"
#include <future>
#include "math.h"
//#define DEBUG_SINGLE_THREAD

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

Hit intersect(const Vector& src, const Vector& ray_dir, const BoundVolumeHierarchy& bvh)
//Takes a source point and ray direction, checks if it intersects any object
//returns hit struct which contains 'metadata' about the intersection.
{
    Hit hit;
    hit.src = src;
    hit.ray_dir= normalise(ray_dir);
    hit.t=-1;
    hit.obj = nullptr;

    GObject::intersection inter = bvh.intersect(src+0.0001*hit.ray_dir, hit.ray_dir, 0);
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

//Color shade(const Hit& hit,
//            int reflection_count,
//            const Sampler& ha1,
//            const Sampler& ha2,
//            const BoundVolumeHierarchy& bvh,
//            const Config& config,
//            const std::vector<std::shared_ptr<GObject>>& objects,
//            const std::vector<Light>& lights)
//{
//    Color c = Color(0, 0, 0);
//
//    if(hit.obj == nullptr || hit.t == -1)
//    {
//        return c;
//    }
//
//    if(hit.obj != nullptr && hit.obj->emission.r>0 && hit.n.dot(hit.ray_dir)< 0) //bad check to see if emissive. && hit.obj->emission.r >0 && hit.n.dot(hit.ray_dir)<0
//    {
//        return hit.obj->emission/255;
//        //c = c+ (hit.obj->emission)/255;
//    }
//
//    if (reflection_count> config.max_reflections)
//    {
//        return c;
//    }
//
//    Vector p = hit.src + hit.t * hit.ray_dir; //hit point
//    Vector n = hit.n; //hit.obj->normal(p);
//    //Vector v = hit.src - p; //vector from point to viewer
//
//    if(hit.obj->brdf == 0 || hit.obj->brdf == 1)
//        //diffuse object
//    {
//        //indirect illumination
//        Vector v1, v2;
//        Utility::create_orthonormal_basis(n, v1, v2);
//
//        Vector sample_dir = Utility::cosine_weighted_hemisphere(ha1.next(), ha2.next(), hit.n); ////can replace with halton series etc in the future.
//        Vector transformed_dir;
//        //I could use my matrix class here but this will save some time on construction/arithmetic maybe...
//        transformed_dir.x = Vector(v1.x, v2.x, n.x).dot(sample_dir);
//        transformed_dir.y = Vector(v1.y, v2.y, n.y).dot(sample_dir);
//        transformed_dir.z = Vector(v1.z, v2.z, n.z).dot(sample_dir);
//        double cos_t = transformed_dir.dot(n);
//        Hit diffuse_relfec_hit = intersect(p, transformed_dir, bvh);
//        if(diffuse_relfec_hit.t != -1 && diffuse_relfec_hit.n.dot(transformed_dir)< 0)//Inbound ray hits correct face (outbound normal vector)
//        {
//            Color diffuse_reflec_color = shade(diffuse_relfec_hit,reflection_count+1, ha1, ha2, bvh, config, objects, lights);
//            double divisor =std::max(1.0,diffuse_relfec_hit.t*diffuse_relfec_hit.t);
//            c = c + diffuse_reflec_color*hit.color/(divisor*255);//idk where 0.1 comes from.cos_t*
//        }
//
//        //direct illumination
//        //for(unsigned int i = 0; i < lights.size(); i++)
//        for(unsigned int i = 0; i < objects.size(); i++)
//        {
//            if(objects[i]->emission.r > 0 || objects[i]->emission.g !=0 || objects[i]->emission.b != 0)
//            {
//                Vector light_point = objects[i]->get_random_point(ha2.next(), ha1.next());//lights[i].position;//
//                Vector s =  light_point- p;
//
//                //Inbound ray hits correct face (outbound normal vector)
//
//                double dist = s.abs();
//                s = normalise(s);
//
//                Hit shadow = intersect(p, s, bvh); //0.001 offset to avoid collision withself //+0.001*s
//
//                //old if statement back in day of point light source.if(shadow.obj == nullptr || shadow.obj == objects[i] || shadow.t < 0.0001 || shadow.t > dist-0.0001)//
//                //the object is not occluded from the light.
//                double cosine_term  = shadow.n.dot(s) *-1; //term used to simulate limb darkening?
//                if(shadow.obj == objects[i].get() && cosine_term>0)
//                {
//                    if(s.dot(n)> 0 ) // light is on right side of the face of obj normal
//                    {
//                        //diffuse
//                        double divisor = std::max(1.0, shadow.t*shadow.t);
//                        c = c + hit.color *cosine_term* objects[i]->emission * s.dot(n)/(divisor * 255 * 255); // lights[i].color
//
//                        if(hit.obj->brdf == 1)
//                            //diffuse object with specular...
//                        {
//                            Vector h = normalise(s + normalise(-1 * hit.t * hit.ray_dir));
//                            double val = h.dot(n)/h.abs();
//                            c = c + cosine_term*objects[i]->emission* pow(val, hit.obj->shininess)/(divisor*255);
//
//                        }
//                    }
//                }
//            }
//        }
//    }else if (hit.obj->brdf == 2)
//        //glass
//    {
//        double n_1 = 1;
//        double n_2 = 1.6;
//        double cos_angle = hit.n.dot(hit.ray_dir);
//        Vector next_ray;
//        double PR;
//        double crit_angle = -1;
//        if (cos_angle>0)
//            //ray incident on the obj
//        {
//            std::swap(n_1, n_2);
//            crit_angle = asin(n_1/n_2);
//        }
//        double angle = acos(abs(cos_angle));
//        if(crit_angle != -1 && angle > crit_angle)
//        {
//            next_ray = normalise(hit.ray_dir - hit.n * 2  *cos_angle);
//        }else
//        {
//            PR = Utility::schlick_fresnel(abs(cos_angle), n_1, n_2);
//
//
//            if(ha1.next() < PR)
//                //reflection
//            {
//                next_ray = normalise(hit.ray_dir - hit.n * 2  *cos_angle);
//            }else
//                //refraction
//            {
//                next_ray = Utility::snells_law(hit.ray_dir, hit.n, cos_angle, n_1, n_2);
//            }
//        }
//
//
//        Hit trace_hit = intersect(p, next_ray, bvh);
//        if(trace_hit.t != -1)
//        {
//            Color color = shade(trace_hit, reflection_count+1, ha1, ha2, bvh, config, objects, lights);
//            c = c + 0.9*color*hit.color/255; //factor of 0.9 for attenuation
//        }
//        //copy paste for direct illumination...
//        for(unsigned int i = 0; i < objects.size(); i++)
//        {
//            if(objects[i]->emission.r > 0 || objects[i]->emission.g !=0 || objects[i]->emission.b != 0)
//            {
//                Vector light_point = objects[i]->get_random_point(ha2.next(), ha1.next());//lights[i].position;//
//                Vector s =  light_point- p;
//                double dist = s.abs();
//                s = normalise(s);
//
//                Hit shadow = intersect(p, s, bvh); //0.001 offset to avoid collision withself //+0.001*s
//
//                if(shadow.obj == nullptr || shadow.obj == objects[i].get() || shadow.t < 0.0001 || shadow.t > dist-0.0001)//
//                    //the object is not occluded from the light.
//                {
//                    if(s.dot(n)>= 0 ) // light is on right side of the face of obj normal
//                    {
//                        //diffuse
//                        //c = c + hit.color * objects[i]->emission * s.dot(n)/(255*255); // lights[i].color
//
//
//                        //diffuse object with specular...
//                        {
//                            //Vector h = normalise(s + normalise(-1*hit.t * hit.ray_dir));
//                            //double val = h.dot(n)/h.abs();
//                            //c = c + objects[i]->emission* pow(val, hit.obj->shininess)/(255);
//
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    return c;
//}

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
    Vector o = origin; //copy
    Vector d = ray_dir; //copy
    Color c;
    Color weight = Color(1,1,1);
    bool ignore_direct = false;
    depth = 0;

    while (true)
    {
        if (depth > config.max_reflections)
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
        if(hit.obj->is_light() && n_dot_ray < 0)
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
        if(hit.obj->brdf==GObject::BRDF::MIRROR)
        {
            ignore_direct = false;
        }else
        {
            ignore_direct = true;
        }
        //ignore_direct = !mat.is_specular;
        Vector new_ray_dir;
        if(hit.obj->brdf==GObject::BRDF::PHONG_DIFFUSE)
            //diffuse
        {
            new_ray_dir = Utility::uniform_hemisphere(ha1.next(), ha2.next(), hit.n);
        }
        else if(hit.obj->brdf==GObject::BRDF::MIRROR)
        {
            new_ray_dir = normalise(hit.ray_dir - hit.n * 2  *n_dot_ray);
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
}