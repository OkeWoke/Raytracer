#pragma once

class Sampler
{
    public:
        Sampler();
        virtual double next() = 0;
        virtual double nextI(int index) = 0;
};
