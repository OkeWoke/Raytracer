#include "imageArray.h"
#include <string>

ImageArray::ImageArray():WIDTH(500),HEIGHT(500)
{

}

ImageArray::ImageArray(int width, int height):WIDTH(width),HEIGHT(height)
{
    histogram = new int[MAX_VAL];
    pixelMatrix = new Color*[WIDTH];

    for(int i=0;i<WIDTH;i++)
    {
        pixelMatrix[i] = new Color[HEIGHT];
    }

    auto fInit = [this](int x, int y)
    {
        pixelMatrix[x][y] =Color();
    };
    iterate(fInit);
}

ImageArray::~ImageArray()
{
    deleteArray();
}

void ImageArray::clearArray()//resets array to 0
{
    auto  fClear = [this](int x, int y)
    {
        Color* c = &pixelMatrix[x][y];
        c->r = 0;
        c->g = 0;
        c->b = 0;
    };

    iterate(fClear);
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

    auto  fMax = [this, &max_pixel_val](int x, int y)
    {
        double t_max = std::max(std::max(pixelMatrix[x][y].r,pixelMatrix[x][y].g),pixelMatrix[x][y].b);
        if (t_max> max_pixel_val)
        {
            max_pixel_val = t_max;
        }
     };

    iterate(fMax);

    return max_pixel_val;
}

void ImageArray::normalise()
{   //this function will scale all pixel values between 0 and max_val, loss of information/quality occurs.
    double max_pixel_val = findMax();
    std::cout << max_pixel_val << std::endl;
    auto  norm = [this, &max_pixel_val](int x, int y)
    {
        pixelMatrix[x][y] = ((MAX_VAL*pixelMatrix[x][y])/max_pixel_val);
    };
    iterate(norm);
}

void ImageArray::gammaCorrection()
{
    auto gam = [this](int x, int y)
    {
        pixelMatrix[x][y].r =  pow(pixelMatrix[x][y].r, 1/2.2);
        pixelMatrix[x][y].g =  pow(pixelMatrix[x][y].g, 1/2.2);
        pixelMatrix[x][y].b =  pow(pixelMatrix[x][y].b, 1/2.2);
    };
    iterate(gam);
}

void ImageArray::clipTop()
{

    auto  clip = [this](int x, int y)
    {
        int top_val = 300;
        if (pixelMatrix[x][y].r >top_val){pixelMatrix[x][y].r = top_val;}
        if (pixelMatrix[x][y].g > top_val){pixelMatrix[x][y].g = top_val;}
        if (pixelMatrix[x][y].b > top_val){pixelMatrix[x][y].b = top_val;}
    };
    iterate(clip);
}

void ImageArray::iterate(std::function<void(int x, int y)> func)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            func(x,y);
        }
    }
}
