# Software Path Tracer [![Build & Test](https://github.com/OkeWoke/Raytracer/actions/workflows/build-test.yml/badge.svg)](https://github.com/OkeWoke/Raytracer/actions/workflows/build-test.yml)

![Example image of raytraced sphere](docs/example_pathtrace2.png)
![Example image of raytraced Mesh](docs/example_pathtrace3.png)

Initially this was a messy learning project in C++ to make some pretty images, but has since involved into a scope creep toy project.

## Features
- Obj file reader for triangular meshes
- Sphere, Plane and Triangle primitives.
- XML Configuration for scene and render settings
- Octree Based BVH Acceleration Structure - a high level BVH for all objects, and a BVH for each mesh object.
- Well distributed multithreading, each thread works on a pixel at a time, and works on the next available pixel when finished.
- Has direct illumination(Next Event Estimation) and indirect illumination via random sampling.
- Uses some form of phong illumination for shading, ambient, diffuse, specular.
- Smooth shading is done for meshes using interpolation of vertex normals.
- Meshes can also be textured!

## Dependencies
- zlib, libpng, png++ for handling png files (saving render / textures)
- CMarkup for defining scenes using xml files.
- CImg for displaying the render in a window.

