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

Color ImageArray::get_median()
{
    //need to compute histogram, cumulative histogram, return no.pixels/2 hist.
    return Color(0,0,0);
}

double ImageArray::get_mean()
{
    double mean=0;
    auto fmean = [this, &mean](int x, int y)
    {
        mean = mean + pixelMatrix[x][y].luminance();
    };

    iterate(fmean);
    mean =mean/(WIDTH*HEIGHT);
    return mean;
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

void ImageArray::linear_scale(double m, double c)
{
    auto  fscale = [this, &m, &c](int x, int y)
    {
        pixelMatrix[x][y] = pixelMatrix[x][y]*m + Color(c,c,c);
    };
    iterate(fscale);
}

void ImageArray::normalise(double max_val)
{   //this function will scale all pixel values between 0 and max_val, loss of information/quality occurs.
    double max_pixel_val = findMax();
    std::cout << max_pixel_val << std::endl;
    auto  norm = [this, &max_pixel_val, &max_val](int x, int y)
    {
        pixelMatrix[x][y] = ((max_val*pixelMatrix[x][y])/max_pixel_val);
    };
    iterate(norm);
}

void ImageArray::gammaCorrection(double gamma)
{
    auto gam = [this, &gamma](int x, int y)
    {
        pixelMatrix[x][y].r =  pow(pixelMatrix[x][y].r, gamma);
        pixelMatrix[x][y].g =  pow(pixelMatrix[x][y].g, gamma);
        pixelMatrix[x][y].b =  pow(pixelMatrix[x][y].b, gamma);
    };
    iterate(gam);
}

double ImageArray::logAverage()
//computes log average of the luminance of the image
{
    double avg= 0;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            avg = avg + log(1+pixelMatrix[x][y].luminance());
        }
    }

    return exp(avg/(WIDTH*HEIGHT));
}

void ImageArray::reinhardToneMap()
{
    double log_avg = logAverage();
    std::cout <<"Log_avg " << log_avg << std::endl;
    double a = 0.76;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            double L_out = a*pixelMatrix[x][y].luminance()/log_avg;
            L_out = L_out/(1+L_out);
            pixelMatrix[x][y].r = pixelMatrix[x][y].r *MAX_VAL*L_out;
            pixelMatrix[x][y].g = pixelMatrix[x][y].g *MAX_VAL*L_out;
            pixelMatrix[x][y].b = pixelMatrix[x][y].b *MAX_VAL*L_out;
        }
    }
}

void ImageArray::clipTop()
{

    auto  clip = [this](int x, int y)
    {
        int top_val = MAX_VAL;
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
