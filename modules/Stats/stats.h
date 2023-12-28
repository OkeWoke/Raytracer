#pragma once
#include <stdlib.h>
#include <atomic>

struct Stats
{
    uint64_t numPrimaryRays = 0;
    std::atomic<uint64_t> numRayTrianglesTests;
    std::atomic<uint64_t> numRayTrianglesIsect;

    Stats()
    {
        numRayTrianglesTests.store(0);
        numRayTrianglesIsect.store(0);
    }
    void clear()
    {
        numPrimaryRays = 0;
        numRayTrianglesTests.store(0);
        numRayTrianglesIsect.store(0);
    }
};

extern Stats stats;
