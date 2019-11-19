# Raytracer

![Example image of raytraced sphere](https://github.com/OkeWoke/Raytracer/blob/master/example_raytrace.png)

This is a raytracer I initially started over a couple days. 

## Features

Supports sphere, plane and triangle primitives.

A .obj file reader that can read triangular meshes.

XML configuration ( used for scene creation too)

Uses some form of phong illumination for shading, ambient, diffuse, specular. (reflections but no refraction yet!)

Looping render display window, allowing for changes in the xml to be reflected asap.

Naive multithreading (distributes render image by rows of pixels to each thread)

Naive intersection (currently each intial ray is checked against every object in the object vector, mesh objects have a single bounding sphere)



## Dependancies
Needs CMarkup, CImg, zlib, libpng and then png++ to compile.

CMarkup is used for deserializing xml that contains config and scene information.

CImg is used for a simple automatically updating display of the render.

Zlib, libpng and png++ are used to save the render into a png format.
