#include "RandomSampler.h"

RandomSampler::RandomSampler(): min(0),max(1)
{
}

RandomSampler::RandomSampler(double min, double max): min(min), max(max)
{
}

double RandomSampler::next()
//returns double value between min and max
{
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<double> distribution(this->min, this->max);
    return distribution(generator);
}

double RandomSampler::nextI(int i)
{
    return 0;
}
