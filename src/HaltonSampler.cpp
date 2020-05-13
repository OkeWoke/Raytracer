#include "HaltonSampler.h"

HaltonSampler::HaltonSampler()
{

}

HaltonSampler::HaltonSampler(int base, int index): base(base), index(index)
{

}

double HaltonSampler::next()
{
    double f =1;
    double r = 0;
    int tmp_index = this->index;

    while (tmp_index > 0)
    {
        f = f/this->base;
        r = r + f * (tmp_index % base);
        tmp_index = (int)floor(tmp_index/base);
    }

    this->index++;
    return r;
}

double HaltonSampler::nextI(int index)
{

    double f =1;
    double r = 0;
    int tmp_index = index;

    while (tmp_index > 0)
    {
        f = f/this->base;
        r = r + f * (tmp_index % base);
        tmp_index = (int)floor(tmp_index/base);
    }

    return r;
}
