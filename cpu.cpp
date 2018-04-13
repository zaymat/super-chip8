#include <iostream>
#include <string>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include "cpu.h"

using namespace std;

/*
    This class represent the CPU of the emulator. It implements all the methods necesserary 
    to execute the majority of opcode (except those which correspond to GPU and Keyboard interrupts).
*/

Cpu::Cpu(string filename, int load_quirk, int shift_quirk){

    srand((unsigned)time(NULL));

    char a;

    // Fonts in memory
    unsigned char chip8_fontset[80] =
        { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    ifstream file;

    // Load the game
    file.open(filename, ios::in | ios::binary);

    // Check whether the file exist
    if(file)
    {
        // Charging fonts in memory
        for (int i = 80; i < 160; i++){
            memory[i] = chip8_fontset[i-80];
        }

        // Loading the game in memory
        for(int i = 512; i<4096; i++){
            if(file.read(&a, sizeof(char))){
                memory[i] = a;
            }
            else{
                memory[i] = 0;
            }
        }

        // Set index and program counter. Note : pc start at address 0x200 (512)
        index = 0;
        pc = 0x200;

        // Wipe unused cases
        for(int i = 0; i<16; i++){
            V[i] = 0;
            key[i] = 0;
        }

        // Set timers
        delay_timer = 60;
        sound_timer = 60;
        this->load_quirk = load_quirk;
        this->shift_quirk = shift_quirk;
    }
    else
    {
        cout << "ERROR: Cannot open the file" << filename << endl;
    }

}

void Cpu::call(unsigned short address){
    this->stack.push(this->pc+2);
    this->pc = address;
}

void Cpu::ret(){
    this->pc = this->stack.top();
    this->stack.pop();
}

void Cpu::jump(unsigned short address){
    this->pc = address;
}

void Cpu::ifEq(unsigned char x, unsigned char val){
    if(this->V[x] == val){
        this->pc += 4;
    }
    else{
        this->pc += 2;
    }
}

void Cpu::ifNotEq(unsigned char x, unsigned char val){
    if(this->V[x] != val){
        this->pc += 4;
    }
    else{
        this->pc += 2;
    }
}

void Cpu::ifEqReg(unsigned char x, unsigned char y){
    if(this->V[x] == this->V[y]){
        this->pc += 4;
    }
    else{
        this->pc += 2;
    }
}

void Cpu::set(unsigned char x, unsigned char val){
    this->V[x] = val;
    this->pc += 2;
}

void Cpu::add(unsigned char x, unsigned char val){
    this->V[x] = this->V[x] + val;
    this->pc += 2;
}

void Cpu::assign(unsigned char x, unsigned char y){
    this->V[x] = this->V[y];
    this->pc += 2;
}

void Cpu::BitOr(unsigned char x, unsigned char y){
    this->V[x] |= this->V[y];
    this->pc += 2;
}

void Cpu::BitAnd(unsigned char x, unsigned char y){
    this->V[x] &= this->V[y];
    this->pc += 2;
}

void Cpu::BitXor(unsigned char x, unsigned char y){
    this->V[x] ^= this->V[y];
    this->pc += 2;
}

void Cpu::addReg(unsigned char x, unsigned char y){
    this->V[15] = (this->V[x] + this->V[y]) / 256;
    this->V[x] =  this->V[y];
    this->pc += 2;
}

void Cpu::subReg(unsigned char x, unsigned char y){
    this->V[15] = (256 + this->V[x] - this->V[y]) / 256;
    this->V[x] = this->V[x] - this->V[y];
    this->pc += 2;
}

void Cpu::shiftRight(unsigned char x, unsigned char y){
    if (this->shift_quirk == 1){
        unsigned char res = this->V[x] >> 1;
        this->V[x] = res;
        this->V[15] = this->V[x] & 0x01;
        this->pc += 2;
    }
    else {
        unsigned char res = this->V[y] >> 1;
        this->V[x] = res;
        this->V[15] = this->V[y] & 0x01;
        this->V[y] = res;
        this->pc += 2;
    }
}

void Cpu::subCopy(unsigned char x, unsigned char y){
    this->V[15] = (256 + this->V[y] - this->V[x]) / 256;
    this->V[x] = this->V[y] - this->V[x];
    this->pc += 2;
}

void Cpu::shiftLeft(unsigned char x, unsigned char y){
    if (this->shift_quirk == 1){
        unsigned char res = this->V[x] << 1;
        this->V[x] = res;
        this->V[15] = (this->V[x] & 0x80) >> 7;
        this->pc += 2;
    }
    else {
        unsigned char res = this->V[y] << 1;
        this->V[x] = res;
        this->V[15] = (this->V[y] & 0x80) >> 7;
        this->V[y] = res;
        this->pc += 2;
    }
}

void Cpu::ifNotEqReg(unsigned char x, unsigned char y){
    if(this->V[x] != this->V[y]){
        this->pc += 4;
    }
    else{
        this->pc += 2;
    }
}

void Cpu::setI(unsigned short val){
    this->index = val;
    this->pc += 2;
}

void Cpu::jumpV0(unsigned short val){
    this->pc = this->V[0] + val;
}

void Cpu::andRand(unsigned char x, unsigned char val){
    char randChar = rand() % 256;
    this->V[x] = randChar & val;
    this->pc += 2;
}

void Cpu::getDelay(unsigned char x){
    this->V[x] = this->delay_timer;
    this->pc += 2;
}

void Cpu::setDelay(unsigned char x){
    this->delay_timer = this->V[x];
    this->pc += 2;
}

void Cpu::setSound(unsigned char x){
    this->sound_timer = this->V[x];
    this->pc += 2;
}

void Cpu::addI(unsigned char x){
    this->index += this->V[x];
    this->pc += 2;
}

void Cpu::getFont(unsigned char x){
    this->index = 5 * this->V[x] + 80;
    this->pc += 2;
}

void Cpu::BCD(unsigned char x){
    this->memory[this->index] = this->V[x] / 100;
    this->memory[this->index + 1] = (this->V[x] / 10) % 10;
    this->memory[this->index + 2] = ((this->V[x]) % 100) % 10;
    this->pc += 2;
}

void Cpu::regDump(unsigned char x){
    for(int i = 0; i <= x; i++){
        this->memory[this->index] = this->V[i];
        this->index += 1;
    }
    if (this->load_quirk == 1){
        this->index -= x+1;
    }
    this->pc += 2;
}

void Cpu::regLoad(unsigned char x){
    for(int i = 0; i <= x; i++){
        this->V[i] = this->memory[this->index];
        this->index += 1;
    }
    if (this->load_quirk == 1){
        this->index -= x+1;
    }
    this->pc += 2;
}

void Cpu::ifKey(unsigned char x){
    if(this->key[this->V[x]] == 1){
        this->pc += 4;
    }
    else{
        this->pc += 2;
    }
}

void Cpu::ifNotKey(unsigned char x){
    if(this->key[this->V[x]] != 1){
        this->pc += 4;
    }
    else{
        this->pc += 2;
    }
}

// Getters and Setter

unsigned char Cpu::getMemory(unsigned short i){
    return this->memory[i];
}

unsigned short Cpu::getPc(){
    return this->pc;
}

void Cpu::setPc(unsigned short pc){
    this->pc = pc;
}

unsigned short Cpu::getIndex(){
    return this->index;
}

unsigned char Cpu::getDelayTimer(){
    return this->delay_timer;
}

unsigned char Cpu::getSoundTimer(){
    return this->sound_timer;
}

void Cpu::updateDelay(){
    this->delay_timer--;
}

void Cpu::updateSound(){
    this->sound_timer--;
}

void Cpu::setKey(unsigned char key, unsigned char val){
    this->key[key] = val;
}

unsigned char Cpu::getReg(unsigned char x){
    return this->V[x];
}
