#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <iostream>
#include <functional>

#include "Color.h"

class ImageArray //A class that handles a 2D array of ints.
{
    private:

        int *histogram;
        void updateHistogram();

        void someFunc(int x, int y);
        void iterate(std::function<void(int x, int y)> func);

    public:
        const int MAX_VAL = 255;
        const int WIDTH;
        const int HEIGHT;
        const int PIXEL_COUNT;

        Color *pixelMatrix;

        ImageArray();
        ~ImageArray();
        ImageArray(int width, int height);

        void deleteArray();
        void clearArray();
        //
        double findMax();
        void normalise(double max_val);
        void gammaCorrection(double gamma);
        void reinhardToneMap();
        double logAverage();
        void clipTop();

        double get_mean();
        Color get_median();
        void linear_scale(double m, double c);
        float* float_array;
        void floatArrayUpdate();
        size_t index(int x, int y) const;
};
