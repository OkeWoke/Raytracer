#pragma once

class Sampler
{
    public:
        virtual double next() const = 0;
        virtual double nextI(int index) const = 0;
        virtual ~Sampler() = default;
};
