#pragma once
#include "Sampler.h"
#include <random>

class RandomSampler : public Sampler
{
    public:
        RandomSampler();
        RandomSampler(double min, double max);

        double next() const override;
        double nextI(int i) const override;

    private:
        double min;
        double max;
};
