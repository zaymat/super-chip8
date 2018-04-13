#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <string>
#include <stack>

class Cpu
{
    public:
        Cpu(std::string filename, int load_quirk, int shift_quirk);
        void call(unsigned short address);
        void ret();
        void jump(unsigned short address);
        void ifEq(unsigned char x, unsigned char val);
        void ifNotEq(unsigned char x, unsigned char val);
        void ifEqReg(unsigned char x, unsigned char y);
        void set(unsigned char x, unsigned char val);
        void add(unsigned char x, unsigned char val);
        void assign(unsigned char x, unsigned char y);
        void BitOr(unsigned char x, unsigned char y);
        void BitAnd(unsigned char x, unsigned char y);
        void BitXor(unsigned char x, unsigned char y);
        void addReg(unsigned char x, unsigned char y);
        void subReg(unsigned char x, unsigned char y);
        void shiftRight(unsigned char x, unsigned char y);
        void subCopy(unsigned char x, unsigned char y);
        void shiftLeft(unsigned char x, unsigned char y);
        void ifNotEqReg(unsigned char x, unsigned char y);
        void setI(unsigned short val);
        void jumpV0(unsigned short val);
        void andRand(unsigned char x, unsigned char val);
        void getDelay(unsigned char x);
        void setDelay(unsigned char x);
        void setSound(unsigned char x);
        void addI(unsigned char x);
        void getFont(unsigned char x);
        void BCD(unsigned char x);
        void regDump(unsigned char x);
        void regLoad(unsigned char x);
        void ifKey(unsigned char x);
        void ifNotKey(unsigned char x);

        // Getters and Setters 
        unsigned char getMemory(unsigned short i);
        unsigned short getPc();
        void setPc(unsigned short pc);
        unsigned short getIndex();
        void updateDelay();
        void updateSound();
        unsigned char getDelayTimer();
        unsigned char getSoundTimer();
        void setKey(unsigned char key, unsigned char val);
        unsigned char getReg(unsigned char x);

    private:
        unsigned short index;
        unsigned short pc;
        unsigned char memory[4096];
        unsigned char V[16];
        std::stack<unsigned short> stack;
        unsigned char delay_timer;
        unsigned char sound_timer;
        unsigned char key[16];
        int shift_quirk;
        int load_quirk;
};

#endif // CPU_H_INCLUDED
