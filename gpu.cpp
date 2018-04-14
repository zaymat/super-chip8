#include <iostream>
#include <SFML/Graphics.hpp>

#include "gpu.h"

using namespace std;

/*
    This class represent the GPU of the emulator. It implements all the methods necesserary 
    to execute the  GPU and Keyboard interrupts opcodes.
*/

Gpu::Gpu(std::string filename){
    string title;
    title = "Chip 8 : " + filename;

    // Create the window
    this->window.create(sf::VideoMode(1280, 640), title);
    this->window.clear(sf::Color::Black);


    // Create CHIP 8 pixels. gfx stores the state of the pixel and pixels the SFML squares
    // representing the pixels (there, the screen is scaled by 10 to be more visible)
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            gfx[i][j] = 0;
            pixels[i][j].setSize(sf::Vector2f(10, 10));
            pixels[i][j].setFillColor(sf::Color(0, 0, 0));
            pixels[i][j].setPosition(sf::Vector2f(i*10,j*10));
            this->window.draw(this->pixels[i][j]);
        }
    }
    this->isExtended = true;
    this->window.display();
}

// Function that draw in Xor mode
unsigned char Gpu::draw(unsigned char x, unsigned char y, unsigned char height, vector<unsigned char> sprite){
    unsigned char VF(0);
    int xi,yi;

    for (int j = 0; j < height; j++)
    {
        yi = (y + j);
        for(int i = 0; i < 8; i++)
        {
            xi = (x + i);
            if((sprite[j] & (0x80 >> i)) != 0)
            {
                if (x < 64*(1 + this->isExtended) && y < 32*(1+this->isExtended)){
                    if(this->gfx[xi][yi] == 1){
                        VF = 1;                   
                    }
                    this->gfx[xi][yi] ^= 1;
                }
            }
        }
    }

    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            if(this->gfx[i][j] == 0){
                this->pixels[i][j].setFillColor(sf::Color(0, 0, 0));
                this->window.draw(this->pixels[i][j]);
            }
            else{
                this->pixels[i][j].setFillColor(sf::Color(255, 255, 255));
                this->window.draw(this->pixels[i][j]);
            }
            
        }
    }

    this->window.display();

    return VF;

}

unsigned char Gpu::SChipDraw(unsigned char x, unsigned char y, std::vector<unsigned short> sprite){
    unsigned char VF(0);
    int xi,yi;

    for (int j = 0; j < 16; j++)
    {
        yi = (y + j);
        for(int i = 0; i < 16; i++)
        {
            xi = (x + i);
            if((sprite[j] & (0x8000 >> i)) != 0)
            {
                if (x < 64*(1 + this->isExtended) && y < 32*(1+this->isExtended)){
                    if(this->gfx[xi][yi] == 1){
                        VF = 1;                   
                    }
                    this->gfx[xi][yi] ^= 1;
                }
            }
        }
    }

    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            if(this->gfx[i][j] == 0){
                this->pixels[i][j].setFillColor(sf::Color(0, 0, 0));
                this->window.draw(this->pixels[i][j]);
            }
            else{
                this->pixels[i][j].setFillColor(sf::Color(255, 255, 255));
                this->window.draw(this->pixels[i][j]);
            }
            
        }
    }

    this->window.display();

    return VF;
}


// Scroll the screen N lines down
void Gpu::scrollDown(unsigned char n){
    for (int j = 63; j >= n; j--){
        for (int i = 0; i < 128; i++){
            this->gfx[i][j] = this->gfx[i][j-n];
        }
    }

    for (int i = 0; i < 128; i++){
        for(int j = 0; j < n; j++){
            this->gfx[i][j] = 0;
        }
    }

    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            if(this->gfx[i][j] == 0){
                this->pixels[i][j].setFillColor(sf::Color(0, 0, 0));
                this->window.draw(this->pixels[i][j]);
            }
            else{
                this->pixels[i][j].setFillColor(sf::Color(255, 255, 255));
                this->window.draw(this->pixels[i][j]);
            }
            
        }
    }

    this->window.display();
}

// Scroll the screen 4 pixels left
void Gpu::scrollLeft(){
    for (int j = 0; j < 64; j++){
        for (int i = 127; i >= 4; i--){
            this->gfx[i][j] = this->gfx[i-4][j];
        }
    }

    for (int i = 0; i < 4; i++){
        for(int j = 0; j < 64; j++){
            this->gfx[i][j] = 0;
        }
    }

    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            if(this->gfx[i][j] == 0){
                this->pixels[i][j].setFillColor(sf::Color(0, 0, 0));
                this->window.draw(this->pixels[i][j]);
            }
            else{
                this->pixels[i][j].setFillColor(sf::Color(255, 255, 255));
                this->window.draw(this->pixels[i][j]);
            }
            
        }
    }

    this->window.display();
}

// Scroll the screen 4 pixels right
void Gpu::scrollRight(){
    for (int j = 0; j < 64; j++){
        for (int i = 0; i < 124; i++){
            this->gfx[i][j] = this->gfx[i+4][j];
        }
    }

    for (int i = 124; i < 128; i++){
        for(int j = 0; j < 64; j++){
            this->gfx[i][j] = 0;
        }
    }

    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            if(this->gfx[i][j] == 0){
                this->pixels[i][j].setFillColor(sf::Color(0, 0, 0));
                this->window.draw(this->pixels[i][j]);
            }
            else{
                this->pixels[i][j].setFillColor(sf::Color(255, 255, 255));
                this->window.draw(this->pixels[i][j]);
            }
            
        }
    }

    this->window.display();
}

// Check whether the window is openned
bool Gpu::isOpen(){
    return this->window.isOpen();
}

// Wipe the screen
void Gpu::clear(){
    this->window.clear();
    for(int i = 0; i < 128; i++){
        for(int j = 0; j < 64; j++){
            this->pixels[i][j].setFillColor(sf::Color(0, 0, 0));
            this->gfx[i][j] = 0;
            this->window.draw(this->pixels[i][j]);     
        }
    }
    this->window.display();
}

// Wait for an event to occur
unsigned char Gpu::waitEvent(sf::Event *event){
    while(this->window.waitEvent(*event)){
        if(event->type == sf::Event::Closed && this->window.waitEvent(*event) == true){
        this->window.close();
        }
        if(event->type == sf::Event::KeyPressed){
            switch(event->key.code){
                case sf::Keyboard::Num1:
                    return 0x01;
                
                case sf::Keyboard::Num2:
                    return 0x02;
                
                case sf::Keyboard::Num3:
                    return 0x03;

                case sf::Keyboard::Num4:
                    return 0x0C;

                case sf::Keyboard::A:
                    return 0x04;
                
                case sf::Keyboard::Z:
                    return 0x05;
                
                case sf::Keyboard::E:
                    return 0x06;

                case sf::Keyboard::R:
                    return 0x0D;

                case sf::Keyboard::Q:
                    return 0x07;
                
                case sf::Keyboard::S:
                    return 0x08;
                
                case sf::Keyboard::D:
                    return 0x09;

                case sf::Keyboard::F:
                    return 0x0E;
                
                case sf::Keyboard::X:
                    return 0x00;
                
                case sf::Keyboard::C:
                    return 0x0B;

                case sf::Keyboard::V:
                    return 0x0F;

                case sf::Keyboard::W:
                    return 0x0A;
                
                default:
                    break;
            }
        }
    }

    return 0xFF;
}

// Check if an event occured and return its value
unsigned short Gpu::pollEvent(sf::Event *event){
    bool check = this->window.pollEvent(*event);
    if(event->type == sf::Event::Closed && check == true){
        this->window.close();
    }
    if(event->type == sf::Event::KeyReleased){
        switch(event->key.code){
            case sf::Keyboard::Num1:
                return 0x0001;
            
            case sf::Keyboard::Num2:
                return 0x0002;
            
            case sf::Keyboard::Num3:
                return 0x0003;

            case sf::Keyboard::Num4:
                return 0x000C;

            case sf::Keyboard::A:
                return 0x0004;
            
            case sf::Keyboard::Z:
                return 0x0005;
            
            case sf::Keyboard::E:
                return 0x0006;

            case sf::Keyboard::R:
                return 0x000D;

            case sf::Keyboard::Q:
                return 0x0007;
            
            case sf::Keyboard::S:
                return 0x0008;
            
            case sf::Keyboard::D:
                return 0x0009;

            case sf::Keyboard::F:
                return 0x000E;
            
            case sf::Keyboard::W:
                return 0x000A;
            
            case sf::Keyboard::X:
                return 0x0000;
            
            case sf::Keyboard::C:
                return 0x000B;

            case sf::Keyboard::V:
                return 0x000F;

            default:
                return 0x00FF;
        }
    }
    else if(event->type == sf::Event::KeyPressed){
       switch(event->key.code){
            case sf::Keyboard::Num1:
                return 0x0101;
            
            case sf::Keyboard::Num2:
                return 0x0102;
            
            case sf::Keyboard::Num3:
                return 0x0103;

            case sf::Keyboard::Num4:
                return 0x010C;

            case sf::Keyboard::A:
                return 0x0104;
            
            case sf::Keyboard::Z:
                return 0x0105;
            
            case sf::Keyboard::E:
                return 0x0106;

            case sf::Keyboard::R:
                return 0x010D;

            case sf::Keyboard::Q:
                return 0x0107;
            
            case sf::Keyboard::S:
                return 0x0108;
            
            case sf::Keyboard::D:
                return 0x0109;

            case sf::Keyboard::F:
                return 0x010E;
            
            case sf::Keyboard::W:
                return 0x010A;
            
            case sf::Keyboard::X:
                return 0x0100;
            
            case sf::Keyboard::C:
                return 0x010B;

            case sf::Keyboard::V:
                return 0x010F;

            default:
                return 0x00FF;
        }     
    }

    return 0x00FF;
}

void Gpu::setIsExtended(bool isExtended){
    this->isExtended = isExtended;
}

void Gpu::closeWindow(){
    this->window.close();
}

bool Gpu::getIsExtended(){
    return this->isExtended;
}