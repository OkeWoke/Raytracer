# Path/Raytracer

![Example image of raytraced sphere](https://github.com/OkeWoke/Raytracer/blob/master/example_pathtrace2.png)
![Example image of raytraced Mesh](https://github.com/OkeWoke/Raytracer/blob/master/example_pathtrace3.jpg)

This is a CPU based pathtracer I initially started over a couple days and have continued adding features and improving it as a learning project in commputer graphics.


## Features

Supports sphere, plane primitives.

A .obj file reader that can read triangular meshes.

XML configuration for iamge processing, core count and scene description.

Uses some form of phong illumination for shading, ambient, diffuse, specular. (reflections but no refraction yet!)
Smooth shading is done for meshes using interpolation of vertex normals.
Meshes can also be textured!

Looping render display window, allowing for changes in the xml to be reflected asap.

Well distributed multithreading, each thread works on a pixel at a time, and works on the next available pixel when finished.

Uses an Octree based Bounding Volume Hierarchy acceleration structure, each bounding volume is constructed from 7 plane normals.
Primitives plane and sphere are supported, system has a high level BVH for all objects, each mesh object gets its own BVH too.

GLFW with GL3 and IMGUI render window and UI.

## Dependancies
Needs GLEW, GLFW, IMGUI, CMarkup, zlib, libpng and then png++ to compile.

CMarkup is used for deserializing xml that contains config and scene information.

Zlib, libpng and png++ are used to save the render into a png format, and read texture images.

GLEW/GLFW/IMGUI used for the display window and UI.