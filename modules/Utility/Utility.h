#include <string>
#include <vector>
#include <stdint.h>
#include <atomic>

#pragma once

extern std::atomic<uint64_t> numRayTrianglesTests;
extern std::atomic<uint64_t> numRayTrianglesIsect;
extern uint64_t numPrimaryRays;

class Utility
{
    public:
        Utility();
        static std::vector<std::string> split(const std::string& s, const std::string& delim);

};

