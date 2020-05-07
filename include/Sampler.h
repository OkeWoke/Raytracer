#ifndef SAMPLER_H
#define SAMPLER_H


class Sampler
{
    public:
        Sampler();
        virtual double next() = 0;
};

#endif // SAMPLER_H
