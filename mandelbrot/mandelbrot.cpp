#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <assert.h>
#include <unistd.h>

#include "../src/log_info/log_errors.h"
#include "../src/generals_func/generals.h"
#include "../src/draw/draw.h"

#include "mandelbrot.h"


const uint8_t Green_cf  = 20;
const uint8_t Blue_cf   = 0;
const uint8_t Red_cf    = 25;


//==============================================================================

static void MandelbrotCalc (Mandelbrot_struct *mandelbrot_struct);

static void MandelbrotGetImage (const Mandelbrot_struct *mandelbrot_struct, sf::Image *img);


static void NewCoord (Mandelbrot_struct *mandelbrot_struct, const int mode);

//==============================================================================

int MandelbrotExe(Mandelbrot_struct *mandelbrot_struct)
{
    assert (mandelbrot_struct != nullptr && "mandelbrot is nullptr");

    sf::RenderWindow window(sf::VideoMode(Window_width, Window_hight), "Mandelbrot");

    sf::Image mandelbrot_img;
    mandelbrot_img.create(mandelbrot_struct->width, mandelbrot_struct->hight, sf::Color::Black);

    size_t frame_cnt = 0;
    sf::Clock fps_time;

    int flag_draw_img = 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            flag_draw_img ^= 1;

        if (flag_draw_img)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                mandelbrot_struct->start_y += -Steep_coord;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                mandelbrot_struct->start_y += Steep_coord;    

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                mandelbrot_struct->start_x += -Steep_coord;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                mandelbrot_struct->start_x += Steep_coord;  

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                NewCoord(mandelbrot_struct, ZOOM);
                

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                NewCoord(mandelbrot_struct, UNZOOM);
        }

        MandelbrotCalc(mandelbrot_struct); 

        if (flag_draw_img)
        {
            MandelbrotGetImage(mandelbrot_struct, &mandelbrot_img);
            DrawImage(&window, &mandelbrot_img);
        }
        else
        {
            window.clear();
            window.display();
        }

        PrintFPS(&window, &fps_time, &frame_cnt);
    }
 
    
    return 0;
}

//===============================================================================

int MandelbrotCtor (Mandelbrot_struct *mandelbrot_struct, 
                    const uint32_t hight, const uint32_t width, 
                    const float start_x, const float start_y, const float delta)
{

    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");

    mandelbrot_struct->hight = hight;
    mandelbrot_struct->width = width;

    mandelbrot_struct->start_x = start_x;
    mandelbrot_struct->start_y = start_y;

    mandelbrot_struct->delta   = delta;

    mandelbrot_struct->exit_num = (size_t*) calloc (hight * width, sizeof(size_t));
    if (CheckNullptr(mandelbrot_struct->exit_num))
        return PROCESS_ERROR(MANDELBROT_CTOR_ERR, "struct ctor failed\n");

    return 0;
}

//===============================================================================

int MandelbrotDtor (Mandelbrot_struct *mandelbrot_struct)
{

    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");

    mandelbrot_struct->hight = 0;
    mandelbrot_struct->width = 0;

    mandelbrot_struct->start_x = 0.f;
    mandelbrot_struct->start_y = 0.f;

    mandelbrot_struct->delta   = 0.f;

    free(mandelbrot_struct->exit_num);

    return 0;
}

//===============================================================================

static void MandelbrotCalc (Mandelbrot_struct *mandelbrot_struct)
{
    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");

	for (uint32_t yi = 0; yi < mandelbrot_struct->hight; yi++) 
    {
        for (uint32_t xi = 0; xi < mandelbrot_struct->width; xi++) 
        {
			float x0 = mandelbrot_struct->start_x + ((float)xi) * mandelbrot_struct->delta;
			float y0 = mandelbrot_struct->start_y + ((float)yi) * mandelbrot_struct->delta;

            float x  = x0;
            float y  = y0;
            
            size_t cnt = 0;

            for (; cnt < Counter_limit; cnt++) 
            {
                float x2 = x * x;
                float y2 = y * y;

                float mul_xy = x * y;

                if (x2 + y2 > Rmax) break;

                x = x2 - y2     + x0;
                y = 2  * mul_xy + y0;
            }

            size_t id = yi * mandelbrot_struct->width + xi;
            *(mandelbrot_struct->exit_num + id) = cnt;
        }
    }

    return;
}

//===============================================================================

static void MandelbrotGetImage (const Mandelbrot_struct *mandelbrot_struct, sf::Image *img)
{
    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");
    assert(img               != nullptr && "img is nullptr");

	for (uint32_t yi = 0; yi < mandelbrot_struct->hight; yi++) 
    {
        for (uint32_t  xi = 0; xi < mandelbrot_struct->width; xi++) 
        {
            uint32_t id = yi * mandelbrot_struct->width + xi;
            size_t cnt = *(mandelbrot_struct->exit_num + id);

            if (cnt == Counter_limit)
                img->setPixel(xi, yi, sf::Color::Black);
            else
                img->setPixel(xi, yi, sf::Color(Red_cf * cnt, Green_cf * cnt, Blue_cf * cnt));
        }
    }

    return;
}

//===============================================================================

static void NewCoord (Mandelbrot_struct *mandelbrot_struct, const int mode)
{
    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");

    float dir = 0.f;

    if (mandelbrot_struct->delta > 0 && !IsZero(mandelbrot_struct->delta))
    {
        if (mode == ZOOM)   dir =  1.f;
        if (mode == UNZOOM) dir = -1.f;
    }
    else
        mandelbrot_struct->delta = 0;

    if (mode == UNZOOM && IsZero(mandelbrot_struct->delta))
        dir = -1.f;

    mandelbrot_struct->delta   +=      -Steep_delta * dir;
    mandelbrot_struct->start_x += 3.f * Steep_coord * dir;
    mandelbrot_struct->start_y += 2.f * Steep_coord * dir;
	
    return;
}

//===============================================================================