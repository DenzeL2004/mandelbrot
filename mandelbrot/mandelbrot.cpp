#include <SFML/Graphics.hpp>
#include <assert.h>


#include "mandelbrot.h"

const uint32_t Window_hight     = 800;
const uint32_t Window_width     = 1200;

const uint8_t Green_cf  = 20;
const uint8_t Blue_cf   = 0;
const uint8_t Red_cf    = 25;


const float Start_x = -1.5f + 1.f;
const float Start_y = -1.f;

const float Delta   = 0.0025f;



int main()
{
    sf::RenderWindow window(sf::VideoMode(Window_width, Window_hight), "Mandelbrot");

    sf::Image mandelbrot_img;
    mandelbrot_img.create(Window_width, Window_hight, sf::Color::Black);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        CalcMandelbrot(&mandelbrot_img);        

        sf::Texture mandelbrot_set_texture;
        mandelbrot_set_texture.loadFromImage(mandelbrot_img);

        sf::Sprite mandelbrot_set_sprite;
        mandelbrot_set_sprite.setTexture(mandelbrot_set_texture);
 
        window.draw(mandelbrot_set_sprite);
        window.display();
    }
 
    return 0;
}

//===============================================================================

void CalcMandelbrot (sf::Image *mandelbrot_img)
{
    assert(mandelbrot_img != nullptr && "image is nullptr");

	for (unsigned int yi = Start_y; yi < Window_hight; yi++) 
    {
        for (unsigned int xi = Start_x; xi < Window_width; xi++) 
        {
			float x0 = Start_x + ((float)xi) * Delta;
			float y0 = Start_y + ((float)yi) * Delta;

            float x  = x0;
            float y  = y0;
            
            size_t cnt = 0;

            for (; cnt < Counter_limit; cnt++) 
            {
                float x2 = x * x;
                float y2 = y * y;

                if (x2 + y2 > Rmax) break;

				y = 2 * x * y + y0;
                x = x2 - y2   + x0;
            }

            if (cnt == Counter_limit)
                mandelbrot_img -> setPixel(xi, yi, sf::Color::Black);
            else 
                mandelbrot_img -> setPixel(xi, yi, sf::Color(Red_cf * cnt, Green_cf * cnt, Blue_cf * cnt));

        }
    }

    return;
}

