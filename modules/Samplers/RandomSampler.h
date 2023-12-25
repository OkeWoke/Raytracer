#pragma once
#include "Sampler.h"
#include <random>

class RandomSampler : public Sampler
{
    public:
        RandomSampler();
        RandomSampler(double min, double max);

        double next();
        double nextI(int i);

    private:
        double min;
        double max;
};
