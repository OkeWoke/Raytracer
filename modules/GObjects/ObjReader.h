#pragma once
#include <string>
#include <vector>
#include "Vector.h"
#include "Triangle.h"
#include "Matrix.h"

struct ObjContents
{
    std::vector<Vector> vertices;
    std::vector<Vector> vn;
    std::vector<Vector> vt;
    std::vector<std::shared_ptr<Triangle>> triangles;
};

ObjContents obj_reader(const std::string& filename, const Matrix& mat=Matrix());