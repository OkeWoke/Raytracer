

#ifndef IMAGE_ARRAY
#define IMAGE_ARRAY
#include <string>
#include <ctime>
#include <cmath>
#include "Color.h"
#include<iostream>

#include<functional>
class ImageArray //A class that handles a 2D array of ints.
{
    private:
        const int MAX_VAL = 255;
        int *histogram;
        void updateHistogram();
        double findMax();
        void someFunc(int x, int y);
        void iterate(std::function<void(int x, int y)> func);

    public:
        const int WIDTH;
        const int HEIGHT;

        Color **pixelMatrix;
        ImageArray(int width, int height);

        void deleteArray();
        void clearArray();
        //
        void normalise();
        void gammaCorrection();

        void clipTop();

};
#endif
