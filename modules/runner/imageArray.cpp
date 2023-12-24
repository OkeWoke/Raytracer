#include "imageArray.h"
#include <string>

ImageArray::ImageArray():WIDTH(500),HEIGHT(500), PIXEL_COUNT(500*500)
{

}

ImageArray::ImageArray(int width, int height):WIDTH(width),HEIGHT(height), PIXEL_COUNT(width*height)
{
    float_array = new float[3*PIXEL_COUNT];
    histogram = new int[MAX_VAL];

    pixelMatrix = new Color[PIXEL_COUNT]; // Temporary continous allocation of memory

    for(int i=0;i<PIXEL_COUNT;i++)
    {
        pixelMatrix[i] = Color();
    }
    std::cout << pixelMatrix[0].r;
}

ImageArray::~ImageArray()
{
    deleteArray();
}

void ImageArray::clearArray()//resets array to 0
{
    for(int i=0;i<PIXEL_COUNT;i++)
    {
        Color* c = &pixelMatrix[i];
        c->r = 0;
        c->g = 0;
        c->b = 0;
    }
}

void ImageArray::deleteArray()
{
    delete [] pixelMatrix;
    delete [] histogram;
    delete [] float_array;
}

Color ImageArray::get_median()
{
    //need to compute histogram, cumulative histogram, return no.pixels/2 hist.
    return Color(0,0,0);
}

double ImageArray::get_mean()
{
    Color temp;
    for(int i=0;i<PIXEL_COUNT;i++)
    {
        temp = temp + pixelMatrix[i];
    }

    return (temp/PIXEL_COUNT).luminance();
}

double ImageArray::findMax()
{
    double max_pixel_val = 0;

    for(int i=0;i<PIXEL_COUNT;i++)
    {
        double t_max = std::max(std::max(pixelMatrix[i].r,pixelMatrix[i].g),pixelMatrix[i].b);
        if (t_max> max_pixel_val)
        {
            max_pixel_val = t_max;
        }
    }

    return max_pixel_val;
}

void ImageArray::linear_scale(double m, double c)
{
    for(int i=0;i<PIXEL_COUNT;i++)
    {
        pixelMatrix[i] = pixelMatrix[i]*m + Color(c,c,c);
    }
}

void ImageArray::normalise(double max_val)
{   //this function will scale all pixel values between 0 and max_val, loss of information/quality occurs.
    double max_pixel_val = findMax();
    std::cout << max_pixel_val << std::endl;
    for(int i=0;i<PIXEL_COUNT;i++)
    {
        pixelMatrix[i] = (max_val*(pixelMatrix[i])/max_pixel_val);
    }
}

void ImageArray::gammaCorrection(double gamma)
{
    for(int i=0;i<PIXEL_COUNT;i++)
    {
        pixelMatrix[i].r =  pow(pixelMatrix[i].r, gamma);
        pixelMatrix[i].g =  pow(pixelMatrix[i].g, gamma);
        pixelMatrix[i].b =  pow(pixelMatrix[i].b, gamma);
    }
}

double ImageArray::logAverage()
//computes log average of the luminance of the image
{
    double avg= 0;
    for (int i = 0; i < PIXEL_COUNT; i++)
    {
        avg = avg + log(1+pixelMatrix[i].luminance());
    }

    return exp(avg/(PIXEL_COUNT));
}

void ImageArray::reinhardToneMap()
{
    double log_avg = logAverage();
    std::cout <<"Log_avg " << log_avg << std::endl;
    double a = 0.76;
    for (int i = 0; i < PIXEL_COUNT; i++)
    {
        double L_out = a*pixelMatrix[i].luminance()/log_avg;
        L_out = L_out/(1+L_out);
        pixelMatrix[i].r = pixelMatrix[i].r *MAX_VAL*L_out;
        pixelMatrix[i].g = pixelMatrix[i].g *MAX_VAL*L_out;
        pixelMatrix[i].b = pixelMatrix[i].b *MAX_VAL*L_out;
    }
}

void ImageArray::clipTop()
{
    for (int i = 0; i < PIXEL_COUNT; i++)
    {
        int top_val = MAX_VAL;
        if (pixelMatrix[i].r >top_val){pixelMatrix[i].r = top_val;}
        if (pixelMatrix[i].g > top_val){pixelMatrix[i].g = top_val;}
        if (pixelMatrix[i].b > top_val){pixelMatrix[i].b = top_val;}
    }
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

void ImageArray::floatArrayUpdate()
{
    double max_pixel_val =  pow(findMax(), 1/2.2);
    for (int i = 0; i < PIXEL_COUNT; i++)
    {
        int k = 3*i;
        float_array[k] = (float)pow(pixelMatrix[i].r, 1/2.2)/max_pixel_val;
        float_array[k+1] = (float)pow(pixelMatrix[i].g, 1/2.2)/max_pixel_val;
        float_array[k+2] = (float)pow(pixelMatrix[i].b, 1/2.2)/max_pixel_val;
    }
}

size_t ImageArray::index(int x, int y) const
{
    return x + WIDTH*y;
}


