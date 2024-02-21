#pragma once

#include "Vector.h"
#include "Color.h"
#include <memory>

class GObject
{
    public:
        enum class BRDF
        {
            PHONG_DIFFUSE,
            PHONG_GLOSSY,
            MIRROR,
        };

        static BRDF brdf_from_string(const std::string& str)
        {
            if(str == "PHONG_DIFFUSE")
            {
                return BRDF::PHONG_DIFFUSE;
            }
            else if(str == "PHONG_GLOSSY")
            {
                return BRDF::PHONG_GLOSSY;
            }
            else if(str == "MIRROR")
            {
                return BRDF::MIRROR;
            }
            else
            {
                throw std::runtime_error("Invalid BRDF type");
            }
        };

        Color color;
        Vector position;
        double shininess = 0;
        double reflectivity = 0;
        Color emission;
        BRDF brdf;
        GObject(){};
        GObject(const GObject& obj): color(obj.color), position(obj.position), shininess(obj.shininess), reflectivity(obj.reflectivity), emission(obj.emission), brdf(obj.brdf){
            //this->bv = std::move(obj.bv);
        };

        bool is_light()
        {
            if(this->emission.r > 0 || this->emission.g > 0 || this->emission.b > 0)
            {
                return true;
            }
            return false;
        };

        virtual ~GObject() {};
        GObject(Vector position): position(position){};
        GObject(Color c, Vector pos, double shininess, double reflectivity):color(c), position(pos), shininess(shininess), reflectivity(reflectivity){};
        GObject(Color c, Vector pos, double shininess, double reflectivity, Color emission, BRDF brdf):color(c), position(pos), shininess(shininess), reflectivity(reflectivity), emission(emission), brdf(brdf){};

        struct intersection
        {
            GObject* obj_ref;
            double t;
            Vector n;
            Color color;
            intersection():obj_ref(nullptr), t(-1),color(Color(-1,-1,-1))
            {
            };

        };

        virtual intersection intersect(const Vector& src, const Vector& d) = 0;
        virtual Vector get_random_point(double val1, double val2) = 0;
        std::shared_ptr<GObject> bv; //we will need to forcefully cast this to BoundVolume type in each usecase...
};
