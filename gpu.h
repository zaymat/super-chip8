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
        void scrollDown(unsigned char n);
        void scrollLeft();
        void scrollRight();
        void setIsExtended(bool isExtended);
        void closeWindow();

    private:
        bool isExtended;
        unsigned char gfx[128][64];
        sf::RectangleShape pixels[128][64];
        sf::RenderWindow window;

};

#endif // GPU_H_INCLUDED
