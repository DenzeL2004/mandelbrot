#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <assert.h>
#include <unistd.h>
#include <immintrin.h>

#include "../src/log_info/log_errors.h"
#include "../src/generals_func/generals.h"
#include "../src/draw/draw.h"

#include "mandelbrot.h"


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
            float scale_cf = mandelbrot_struct->delta * 10;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                mandelbrot_struct->start_y += -scale_cf;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                mandelbrot_struct->start_y += scale_cf;    

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                mandelbrot_struct->start_x += -scale_cf;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                mandelbrot_struct->start_x += scale_cf;  

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

    mandelbrot_struct->exit_num = (int*) calloc (hight * width, sizeof(int));
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

    float start_x = mandelbrot_struct->start_x;
    float start_y = mandelbrot_struct->start_y;
    float delta   = mandelbrot_struct->delta;

    const __m256 Rmax2_  = _mm256_set1_ps(Rmax * Rmax);
    const __m256 Delta_  = _mm256_set1_ps(delta);
    const __m256 It_     = _mm256_set_ps (7 * delta, 6 * delta, 5 * delta, 4 * delta, 
                                          3 * delta, 2 * delta,     delta, 0);

	for (uint32_t yi = 0; yi < mandelbrot_struct->hight; yi++) 
    {
        __m256 x0_ = _mm256_add_ps(_mm256_set1_ps(start_x), It_); 
		__m256 y0_ = _mm256_set1_ps(start_y + ((float)yi) * delta); 

        for (uint32_t xi = 0; xi < mandelbrot_struct->width; xi += 8) 
        {
            __m256 x_  = x0_;
            __m256 y_  = y0_;
            
            __m256i cnt_ = _mm256_set1_epi32(Counter_limit);

            for (uint32_t cnt = 0; cnt < Counter_limit; cnt++) 
            {
                __m256 x2_ = _mm256_mul_ps(x_, x_);
                __m256 y2_ = _mm256_mul_ps(y_, y_);

                __m256 xy_ = _mm256_mul_ps(x_, y_);

                __m256 r2_  = _mm256_add_ps(x2_, y2_);

                __m256 cmp_ = _mm256_cmp_ps(r2_, Rmax2_, _CMP_LT_OQ);
                int mask = _mm256_movemask_ps(cmp_);
                if (!mask) break;
                
                cnt_ = _mm256_sub_epi32(cnt_, _mm256_castps_si256(cmp_));

                x_ = _mm256_add_ps(_mm256_sub_ps(x2_, y2_), x0_);
                y_ = _mm256_add_ps(_mm256_add_ps(xy_, xy_), y0_);
            }

            cnt_ = _mm256_sub_epi32(cnt_, _mm256_set1_epi32(255));

            uint32_t id = yi * mandelbrot_struct->width + xi;
            _mm256_storeu_si256((__m256i*)(mandelbrot_struct->exit_num + id), cnt_);

            x0_= _mm256_add_ps(x0_, _mm256_mul_ps(Delta_, _mm256_set1_ps(8)));
        }
    }

    return;
}

//===============================================================================

static void MandelbrotGetImage (const Mandelbrot_struct *mandelbrot_struct, sf::Image *img)
{
    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");
    assert(img               != nullptr && "img is nullptr");

    const uint8_t Red_cf    = 8;
    const uint8_t Green_cf  = 4;
    const uint8_t Blue_cf   = 4;
    
    const uint8_t Red_offset    = 16;
    const uint8_t Green_offset  = 4;
    const uint8_t Blue_offset   = 8;
    

	for (uint32_t yi = 0; yi < mandelbrot_struct->hight; yi++) 
    {
        for (uint32_t  xi = 0; xi < mandelbrot_struct->width; xi++) 
        {
            uint32_t id = yi * mandelbrot_struct->width + xi;
            int cnt = *(mandelbrot_struct->exit_num + id);

            if (cnt == Counter_limit)
                img->setPixel(xi, yi, sf::Color::Black);
            else
                img->setPixel(xi, yi, sf::Color((uint8_t)(Red_cf   * cnt + Red_offset), 
                                                (uint8_t)(Green_cf * cnt + Green_offset), 
                                                (uint8_t)(Blue_cf  * cnt + Blue_offset)));

        }
    }

    return;
}

//===============================================================================

static void NewCoord (Mandelbrot_struct *mandelbrot_struct, const int mode)
{
    assert(mandelbrot_struct != nullptr && "mandelbrot_struct is nullptr");

    float dir = 0.f;

    float scale_cf  = mandelbrot_struct->delta * Scale_delta;

    if (mandelbrot_struct->delta > 0 && !IsZero(mandelbrot_struct->delta))
    {
        if (mode == ZOOM)   dir =  1.f;
        if (mode == UNZOOM) dir = -1.f;
    }
    else
        mandelbrot_struct->delta = 0;

    if (mode == UNZOOM && IsZero(mandelbrot_struct->delta))
        dir = -1.f;

    mandelbrot_struct->delta   +=      -scale_cf * dir;
    mandelbrot_struct->start_x +=       scale_cf * dir;
    mandelbrot_struct->start_y +=       scale_cf * dir;
	
    return;
}

//===============================================================================