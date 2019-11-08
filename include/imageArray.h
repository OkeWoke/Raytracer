

#ifndef IMAGE_ARRAY
#define IMAGE_ARRAY
#include <string>
#include <ctime>
#include <cmath>
#include "Color.h"
#include<iostream>
class ImageArray //A class that handles a 2D array of ints.
{
    private:
        const int MAX_VAL = 255;
        int *histogram;
        void updateHistogram();
         double findMax();

    public:
        const int WIDTH;
        const int HEIGHT;

        Color **pixelMatrix;
        ImageArray(int width, int height);
        void clearArray();
        void deleteArray();
        void normalise();

};
#endif
