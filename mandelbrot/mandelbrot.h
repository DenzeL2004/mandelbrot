#ifndef _MANDELBROT_H_
#define _MANDELBROT_H_

#include <stdint.h>
#include <stddef.h>

#include <immintrin.h>

//===============================================================================

const float Rmax = 5.0f;             

const size_t Counter_limit = 255;

const float Init_start_x = -1.5f;
const float Init_start_y = -1.f;

const float Init_delta   = 0.0025f;

const float Scale_coord = 10.f;
const float Scale_delta = 0.02f;


const uint32_t Window_hight  = 800;
const uint32_t Window_width  = 1200;

const size_t   Accuracy = 100;

//===============================================================================

const useconds_t Delay = 100000;

//===============================================================================

enum Direct
{
    UNZOOM  = -1,
    ZOOM    =  1,

};

//===============================================================================

enum Mandelbrot_ERR
{
    MANDELBROT_CTOR_ERR     = -1,
    MANDELBROT_DTOR_ERR     = -2,

    MANDELBROT_GET_ERR      = -3,
};

//===============================================================================

struct Mandelbrot_struct
{
    uint32_t width = 0;
    uint32_t hight = 0;

    float start_x = Init_start_x;
    float start_y = Init_start_y;

    float delta   = Init_delta;

    int *exit_num = nullptr;
};

//===============================================================================

int MandelbrotExe(Mandelbrot_struct *mandelbrot_struct);


int MandelbrotCtor (Mandelbrot_struct *mandelbrot_struct, 
                    const uint32_t hight, const uint32_t width, 
                    const float Init_start_x, const float Init_start_y, const float Init_delta);

int MandelbrotDtor (Mandelbrot_struct *mandelbrot_struct);

//===============================================================================

#endif