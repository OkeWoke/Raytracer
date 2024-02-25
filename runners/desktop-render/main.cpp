#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <time.h>
#include <thread>
#include <atomic>

#include "CImg-w.h"
#include "pngpp-w.hpp"

#include "deserialize.h"
#include "render.h"

#include "BoundVolumeHierarchy.h"
#include "HaltonSampler.h"
#include "RandomSampler.h"

#include "stats.h"
#include "imageArray.h"

extern Stats stats;

void draw(ImageArray& img, const std::string& filename, const std::string& stretch);

void modify_scene_from_key_input(unsigned int key, Scene& scene)
{
    auto mod_mat = scene.cam.mat;
    if (key == 119) // W key
    {
        mod_mat = mod_mat * Matrix::translate(scene.cam.n * -0.5);
    } else if (key == 115) // S key
    {
        mod_mat = mod_mat * Matrix::translate(scene.cam.n * 0.5);
    } else if (key == 97) // A key
    {
        mod_mat = mod_mat * Matrix::translate(scene.cam.u * -0.5);
    } else if (key == 100) // D key
    {
        mod_mat = mod_mat * Matrix::translate(scene.cam.u * 0.5);
    } else if (key == 113) // Q key
    {
        mod_mat = mod_mat * Matrix::translate(scene.cam.v * 0.5);
    } else if (key == 101) // E key
    {
        mod_mat = mod_mat * Matrix::translate(scene.cam.v * -0.5);
    } //case for arrow keys
    else if (key == 65362) // UP key
    {
        scene.cam.N += 2;
    } else if (key == 65364) // DOWN key
    {
        scene.cam.N -= 2;
    } else if (key == 65361) // LEFT key
    {
        mod_mat = mod_mat * Matrix::rot_y(0.15);
    } else if (key == 65363) // RIGHT key
    {
        mod_mat = mod_mat * Matrix::rot_y(-0.15);
    }

    scene.config.max_reflections = 1;
    scene.cam.update_camera(mod_mat);
}

int main()
{
    std::cout<<"Loading scene from scene.xml..." << std::endl;
    auto load_start = std::chrono::steady_clock::now();
    std::string rootPath = RootPath;
    std::string renderDest = rootPath + "/renders/";

    Scene scene = deserializeScene(rootPath + "/data/scenes/scene.xml");

    auto load_end = std::chrono::steady_clock::now();
    std::cout<<"Loading completed in: " << (load_end-load_start)/std::chrono::milliseconds(1)<< " (ms)" << std::endl;

    ImageArray img(scene.cam.H_RES, scene.cam.V_RES);
    cimg_library::CImg<float> display_image(scene.cam.H_RES, scene.cam.V_RES,1,3,0);
    cimg_library::CImgDisplay display(display_image, "Oke's Path Tracer!");

    //Creation of BoundVolume Hierarchy
    auto bvh_start = std::chrono::steady_clock::now();
    BoundVolumeHierarchy bvh(scene.objects);
    auto bvh_end = std::chrono::steady_clock::now();
    std::cout<<"BVH Constructed in: " << (bvh_end-bvh_start)/std::chrono::milliseconds(1)<< " (ms)" << std::endl;

    //Creation of samplers used for montecarlo integration.
    RandomSampler sampler1, sampler2;
    //HaltonSampler(7, rand()%5000 + 1503);
    //HaltonSampler(3, rand()%5000 + 5000);

    /////////////////////////////////////// CAST & DISPLAY  CODE /////////////////////////////
    auto cast_start = std::chrono::steady_clock::now();
    std::atomic<unsigned int> key = 0;
    std::thread inputHandler([&display, &key]()
    {
        while (!display.is_closed())
        {
            display.wait(); // Wait for an event
            auto tmp = display.key(); // Get the pressed key
            if (tmp != 0)
            {
                key = tmp;
            }

            if (display.is_keyESC())
            {
                break;
            }
        }
    });

    int s;
    double exponent = 1/4.0;

    const int originalReflectCount = scene.config.max_reflections;
    for(s=0;s<scene.config.spp; s++)
    {
        if (key != 0)
        {
            std::cout << "key changed" << std::endl;
            img.clearArray();
            s = 0;
            modify_scene_from_key_input(key, scene);
            key = 0;
        }
        else
        {
            scene.config.max_reflections = originalReflectCount;
        }
        cast_rays_multithread(scene,
                              img,
                              sampler1,
                              sampler2,
                              bvh);
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

    }



    auto cast_end = std::chrono::steady_clock::now();

    const int orw = 20;
    std::cout << "Casting completed in: "<< std::setw(orw) << (cast_end - cast_start)/std::chrono::milliseconds(1)<< " (ms)"<<std::endl;
    std::cout << "Number of primary rays: " << std::setw(orw+1) << stats.numPrimaryRays << std::endl;
    std::cout << "Number of Triangle Tests: " << std::setw(orw) << stats.numRayTrianglesTests.load() << std::endl;
    std::cout << "Number of Triangle Intersections: " <<std::setw(orw-11) << stats.numRayTrianglesIsect.load() << std::endl;
    std::cout << "Percentage of sucesful triangle tests: " << std::setw(orw-12) << 100*(float) stats.numRayTrianglesIsect.load()/stats.numRayTrianglesTests.load()<< "%" << std::endl;
    std::cout << "----------------------------------------------------------\n\n\n\n"<<std::endl;
    std::cout << "Waiting for window close to save" << std::endl;

    if(inputHandler.joinable())
    {
        inputHandler.join();
    }
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

    auto save_end = std::chrono::steady_clock::now();
    std::cout << "Image Save completed in: "<< std::setw(orw-7) <<(save_end - save_start)/std::chrono::milliseconds(1)<< " (ms)"<<std::endl;
    std::cout << "----------------------------------------\n\n\n\n" << std::endl;

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



