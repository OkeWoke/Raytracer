#include "ObjReader.h"

ObjContents obj_reader(const std::string& filename, const Matrix& mat)
{
    ObjContents obj;

    obj.vt.push_back(Vector()); // why do we do this, I forgot?
    std::ifstream file;
    try {
        file.open(filename);
    } catch (std::system_error) {
        std::cout << "Error reading file" << std::endl;
    }
    if (!file) {
        std::cout << "Unable to open .obj file" << std::endl;
        std::runtime_error("Unable to open .obj file: " + filename);
    }

    std::string line;
    while (getline(file, line)) {
        if (line.length() < 3) { continue; }
        try {
            double x, y, z;
            std::istringstream iss(line.substr(2, 36));
            if (line.substr(0, 2) == "v ") {
                iss >> x >> y >> z;
                obj.vertices.push_back(mat.mult_vec(Vector(x, y, z), 1));
            } else if (line.substr(0, 2) == "vt") {
                iss >> x >> y;
                obj.vt.push_back(Vector(x, y, 0));
            } else if (line.substr(0, 2) == "vn") {
                iss >> x >> y >> z;

                obj.vn.push_back(mat.mult_vec(Vector(x, y, z), 0));
            } else if (line.substr(0, 2) == "f ") {
                auto vec_stoi = [](const std::vector<std::string> &vec) {

                    return std::vector<int>{stoi(vec[0]), stoi(vec[1]), stoi(vec[2])};
                };

                //this vector should be length 3...
                std::vector<std::string> face_points = Utility::split(line.substr(2, 100), " ");
                std::vector<int> i0 = vec_stoi(Utility::split(face_points[0], "/"));
                std::vector<int> i1 = vec_stoi(Utility::split(face_points[1], "/"));
                std::vector<int> i2 = vec_stoi(Utility::split(face_points[2], "/"));
                std::shared_ptr<Triangle> tri =
                        std::make_shared<Triangle>(
                                Triangle(
                                        obj.vertices[i0[0] - 1],
                                        obj.vertices[i1[0] - 1],
                                        obj.vertices[i2[0] - 1],
                                        obj.vt[i0[1] - 1],
                                        obj.vt[i1[1] - 1],
                                        obj.vt[i2[1] - 1],
                                        obj.vn[i0[2] - 1],
                                        obj.vn[i1[2] - 1],
                                        obj.vn[i2[2] - 1]));

                obj.triangles.push_back(tri);
            }

        } catch (std::out_of_range) {
            std::cout << "Error reading .obj file!" << std::endl;
            std::runtime_error("Error reading .obj file: " + filename);
        }
    }
    std::cout << "Triangle count: " << obj.triangles.size() << std::endl;

    return obj;
}