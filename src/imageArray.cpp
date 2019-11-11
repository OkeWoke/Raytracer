#include "imageArray.h"
#include <string>
ImageArray::ImageArray(int width, int height):WIDTH(width),HEIGHT(height)
{
    histogram = new int[MAX_VAL];
    pixelMatrix = new Color*[WIDTH];

    for(int i=0;i<WIDTH;i++)
    {
        pixelMatrix[i] = new Color[HEIGHT];
    }
    for(int y= 0;y<HEIGHT;y++)//instantiating iters in each pixel
    {
        for(int x=0; x<WIDTH;x++)
        {
            pixelMatrix[x][y] =Color();
        }
    }
}

void ImageArray::clearArray()//resets array to 0
{
    for(int y= 0;y<HEIGHT;y++)
    {
        for(int x=0; x<WIDTH;x++)
        {
            Color* c = &pixelMatrix[x][y];
            c->r = 0;
            c->g = 0;
            c->b = 0;
        }
    }
}

void ImageArray::deleteArray()
{
    for(int i=0;i<WIDTH;i++)
    {
        delete [] pixelMatrix[i];
    }
    delete [] pixelMatrix;
    delete [] histogram;
}

double ImageArray::findMax()
{
    double max_pixel_val = 0;
    for (int y = 0; y < HEIGHT; y++)//find max value in the image array
    {
        for (int x = 0; x < WIDTH; x++)
        {
            double t_max = std::max(std::max(pixelMatrix[x][y].r,pixelMatrix[x][y].g),pixelMatrix[x][y].b);
            if (t_max> max_pixel_val)
            {
                max_pixel_val = t_max;
            }
        }
    }
    return max_pixel_val;
}

void ImageArray::normalise()
{   //this function will scale all pixel values between 0 and max_val, loss of information/quality occurs.
    double max_pixel_val = findMax();
    std::cout << max_pixel_val << std::endl;
    for (int y = 0; y < HEIGHT; ++y)//normalising the value
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            pixelMatrix[x][y] = ((MAX_VAL*pixelMatrix[x][y])/max_pixel_val);
        }
    }
}
