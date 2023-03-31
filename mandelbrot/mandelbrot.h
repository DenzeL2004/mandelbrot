#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stddef.h>

const float Rmax = 4.0f;             

const size_t Counter_limit = 255;

struct Mandelbrot_struct
{

    float Start_x = -1.5f + 1.f;
    float Start_y = -1.f;

    float Delta   = 0.0025f;

    size_t *exit_num = nullptr;
};

void CalcMandelbrot (Mandelbrot_struct *mandelbrot_struct);

#endif