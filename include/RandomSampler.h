#include "Sampler.h"
#ifndef RANDOMSAMPLER_H
#define RANDOMSAMPLER_H

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

#endif // RANDOMSAMPLER_H
