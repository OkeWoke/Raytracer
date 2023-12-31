#include "ObjReader.h"

ObjContents obj_reader(const std::string& filename, const Matrix& mat) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << std::endl;
        std::runtime_error("Failed to open file " + filename);
    }
    ObjContents obj;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vector vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            obj.vertices.push_back(mat.mult_vec(vertex, 1));
        } else if (prefix == "vt") {
            Vector texture;
            iss >> texture.x >> texture.y;
            obj.vt.push_back(texture);
        } else if (prefix == "vn") {
            Vector normal;
            iss >> normal.x >> normal.y >> normal.z;
            obj.vn.push_back(mat.mult_vec(normal, 0));
        } else if (prefix == "f") {
            Vector v[3];
            Vector vt[3] = {Vector(), Vector(), Vector()};
            Vector vn[3] = {Vector(), Vector(), Vector()};
            for (int i = 0; i < 3; ++i) {
                std::string vertex;
                iss >> vertex;
                size_t pos1 = vertex.find('/');
                size_t pos2 = vertex.find('/', pos1 + 1);

                v[i] = obj.vertices[std::stoi(vertex.substr(0, pos1)) - 1];
                if (pos1 != std::string::npos) {

                    if (pos1 + 1 != pos2) {
                        vt[i] = obj.vt[std::stoi(vertex.substr(pos1 + 1, pos2 - pos1 - 1)) - 1];
                    }
                    if (pos2 != std::string::npos) {
                        vn[i] = obj.vn[std::stoi(vertex.substr(pos2 + 1)) - 1];
                    }
                }
            }
            std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(Triangle(v, vt, vn));
            obj.triangles.push_back(tri);
        }
    }
    std::cout << "Triangle count: " << obj.triangles.size() << std::endl;

    return obj;
}

