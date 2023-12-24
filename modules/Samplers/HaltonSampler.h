#pragma once

#include "Sampler.h"
#include <cmath>

class HaltonSampler : public Sampler
{
    public:
        HaltonSampler();
        HaltonSampler(int base, int index);
        double next();
        double nextI(int index);

        int base;
        int index;
};
