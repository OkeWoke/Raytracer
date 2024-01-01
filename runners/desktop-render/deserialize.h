#pragma once
#include "Markup-w.h"
#include "Mesh.h"
#include "Camera.h"
#include "GObject.h"
#include "Light.h"
#include "RenderStructs.h"

Scene deserializeScene(const std::string& filename);
MeshConfig deserializeMesh(std::basic_string<char> xmlStr);
Vector deserializeVector(const std::string& sub);
Color deserializeColor(const std::string& sub);
Sphere deserializeSphere(const std::string& strSubDoc);
Plane deserializePlane(const std::string& strSubDoc);
Camera deserializeCamera(const std::string& sub);
Light deserialize(const std::string& strSubDoc);
Light deserializeLight(const std::string& strSubDoc);