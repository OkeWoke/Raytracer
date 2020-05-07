#include "Sampler.h"
#ifndef HALTONSAMPLER_H
#define HALTONSAMPLER_H
#include <cmath>

class HaltonSampler : public Sampler
{
    public:
        HaltonSampler();
        HaltonSampler(int base, int index);
        double next();
    protected:

    private:
        int base;
        int index;
};

#endif // HALTONSAMPLER_H
