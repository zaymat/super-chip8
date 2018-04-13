#ifndef GPU_H_INCLUDED
#define GPU_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string>

class Gpu
{
    public:
        Gpu(std::string filename);
        unsigned char draw(unsigned char x, unsigned char y, unsigned char height, std::vector<unsigned char> sprite);
        bool isOpen();
        void clear();
        unsigned char waitEvent(sf::Event *event);
        unsigned short pollEvent(sf::Event *event);
        

    private:
        unsigned char gfx[64][32];
        sf::RectangleShape pixels[64][32];
        sf::RenderWindow window;

};

#endif // GPU_H_INCLUDED
