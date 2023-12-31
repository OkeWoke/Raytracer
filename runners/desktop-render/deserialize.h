#pragma once
#include "Markup-w.h"
#include "Mesh.h"
#include "Camera.h"
#include "GObject.h"
#include "Light.h"
#include "RenderStructs.h"

void deserialize(const std::string& filename, Scene& scene);
MeshConfig deserializeMesh(std::basic_string<char> xmlStr);