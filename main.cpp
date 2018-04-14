#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <unistd.h>

#include "cpu.h"
#include "gpu.h"

using namespace std;

int main (int argc, char *argv[])
{
    // Notes : registers are named V0 -> V15
    //         NNN is a 12 bits address
    //         val is a 8 bits value
    //         x,y are 8 bits registers ids

    // Declare cpu arguments
    string file = "";
    int load_quirk = 0;
    int shift_quirk = 0;

    // Parse the arguments
    // Must be provided :
    //              - path to the game
    // Can be provided :
    //              - s : set shift_quirk to true
    //              - l : set load_quirk to true

    if (argc < 2 ){
        cout << "Wrong number of argument: at least 1 is required but " << argc-1 << " was given" << endl;
        return 1;
    }
    else if (argc > 4){
        cout << "Wrong number of argument: max 3 can be provided but " << argc-1 << " were given" << endl;
        return 1;
    }
    else{
        for(int i = 1; i < argc; i++){
            // Convert args to string
            string arg = string(argv[i]);

            if (arg == "-l"){
                load_quirk = 1;
            }
            else if (arg == "-s"){
                shift_quirk = 1;
            }
            else if (arg == "-ls"){
                shift_quirk = 1;
                load_quirk = 1;
            }
            else if (arg == "-sl"){
                shift_quirk = 1;
                load_quirk = 1;
            }
            else if (arg == "--help"){
                cout << "Welcome to CHIP 8 emulator help page \n\n"
                "You must provide the path to the game file\n\n"
                "Flags :\n"
                "\t -s : set shift_quirk to true (must be enabled for some games)\n"
                "\t -l : set load_quirk to true (must be enabled for some games)\n" << endl;
                return 0;
            }
            else if (arg == "-h"){
                cout << "Welcome to CHIP 8 emulator help page \n\n"
                "You must provide the path to the game file\n\n"
                "Flags :\n"
                "\t -s : set shift_quirk to true (must be enabled for some games)\n"
                "\t -l : set load_quirk to true (must be enabled for some games)\n" << endl;
                return 0;
            }
            else if ((char)arg[0] == '-'){
                cout << "Unknown argument " << arg << ". Please see help page (-h or --help)" << endl;
                return 1;
            }
            else{
                file = arg;
            }
        }
    }

    

    // Creating Cpu and Gpu objects
    Cpu cpu(file, load_quirk, shift_quirk);
    Gpu gpu(file);
    
    unsigned char x, y, val;
    unsigned short address, opcode, index;
    unsigned char height;
    vector<unsigned char> sprite;
    vector<unsigned short> SChipSprite;


    // Event Loop (note : events are caught by the Gpu)
    while(gpu.isOpen()){

        sf::Event event;
        
        // Fetching the opcode
        opcode = cpu.getMemory(cpu.getPc()) << 8 | cpu.getMemory(cpu.getPc() + 1);
        printf("%x:%x\n", opcode, cpu.getPc());

        // Parsing the opcode
        switch(opcode & 0xF000){
            case 0x0000:
                switch(opcode & 0x00FF){
                    case 0x00E0:
                    // Wipe the screen
                        gpu.clear();
                        cpu.setPc(cpu.getPc() + 2);
                        break;

                    // Return to the previous function on the stack
                    case 0x00EE:
                        cpu.ret();
                        break;
                    
                    // Scroll the screen 4 pixels right
                    case 0x00FB:
                        gpu.scrollRight();
                        cpu.setPc(cpu.getPc() + 2);
                        break;
                    
                    // Scroll the screen 4 pixels left
                    case 0x00FC:
                        gpu.scrollLeft();
                        cpu.setPc(cpu.getPc() + 2);
                        break;

                    // Exit the program
                    case 0x00FD:
                        gpu.closeWindow();
                        cpu.setPc(cpu.getPc() + 2);
                        break;
                    
                    // Set isExtended to false
                    case 0x00FE:
                        gpu.setIsExtended(false);
                        cpu.setPc(cpu.getPc() + 2);
                        break;
                    
                    // Set isExtended to true
                    case 0x00FF:
                        gpu.setIsExtended(true);
                        cpu.setPc(cpu.getPc() + 2);
                        break;

                    default:
                        if ((opcode & 0x00F0) == 0x00C0){
                            // Scoll the screen N pixels down
                            val = (opcode & 0x000F);
                            gpu.scrollDown(val);
                            cpu.setPc(cpu.getPc() + 2);
                        }
                        else{
                        // Call the program at address NNN
                            address = opcode & 0x0FFF;
                            cpu.call(address);
                        } 
                        break;
                }
                break;

            case 0x1000:
            // Jump to address NNN
                address = opcode & 0x0FFF;
                cpu.jump(address);
                break;

            case 0x2000:
            // Call the function at address NNN
                address = opcode & 0x0FFF;
                cpu.call(address);
                break;
            
            case 0x3000:
            // Pass if V[x] == val
                x = (opcode & 0x0F00) >> 8;
                val = opcode & 0x00FF;
                cpu.ifEq(x, val);
                break;
            
            case 0x4000:
            // Pass if V[x] != val
                x = (opcode & 0x0F00) >> 8;
                val = opcode & 0x00FF;
                cpu.ifNotEq(x, val);
                break;

            case 0x5000:
            // Pass if V[x] == V[y]
                x = (opcode & 0x0F00) >> 8;
                y = (opcode & 0x00F0) >> 4;
                cpu.ifEqReg(x, y);
                break;
            
            case 0x6000:
            // Set V[x] to val
                x = (opcode & 0x0F00) >> 8;
                val = opcode & 0x00FF;
                cpu.set(x, val);
                break;

            case 0x7000:
            // Increment V[x] by val
                x = (opcode & 0x0F00) >> 8;
                val = opcode & 0x00FF;
                cpu.add(x, val);
                break;
            
            case 0x8000:
                x = (opcode & 0x0F00) >> 8;
                y = (opcode & 0x00F0) >> 4;
                switch(opcode & 0x000F){
                    case 0x0000:
                    // assign V[y] to V[x]
                        cpu.assign(x, y);
                        break;

                    case 0x0001:
                    // Perform bitwise Or on V[x] and V[y] and store it to V[x]
                        cpu.BitOr(x, y);
                        break;

                    case 0x0002:
                    // Perform bitwise And on V[x] and V[y] and store it to V[x]
                        cpu.BitAnd(x, y);
                        break;
                    
                    case 0x0003:
                    // Perform bitwise Xor on V[x] and V[y] and store it to V[x]
                        cpu.BitXor(x, y);
                        break;
                    
                    case 0x0004:
                    // Add V[x] to V[y] and store it to V[x]
                        cpu.addReg(x, y);
                        break;

                    case 0x0005:
                    // Substract V[y] from V[x] and store it to V[x]
                        cpu.subReg(x, y);
                        break;

                    case 0x0006:
                    // Divide V[y] by 2 and store the result to V[x]
                        cpu.shiftRight(x, y);
                        break;
                    
                    case 0x0007:
                    // Substract V[x] from V[y] and store it to V[x]
                        cpu.subCopy(x, y);
                        break;
                    
                    case 0x000E:
                    // Multiply V[y] by 2 and store it to V[x]
                        cpu.shiftLeft(x, y);
                        break;
            
                    default:
                        break;
                }
                break;
        
            case 0x9000:
            // Pass if V[x] != V[y]
                x = (opcode & 0x0F00) >> 8;
                y = (opcode & 0x00F0) >> 4;
                cpu.ifNotEqReg(x, y);
                break;

            case 0xA000:
            // Set index to NNN
                address = opcode & 0x0FFF;
                cpu.setI(address);
                break;

            case 0xB000:
            // Jump to address NNN + V0
                address = opcode & 0x0FFF;
                cpu.jumpV0(address);
                break;
            
            case 0xC000:
            // Store a random number in V[x]
                x = (opcode & 0x0F00) >> 8;
                val = opcode & 0x00FF;
                cpu.andRand(x, val);
                break;
            
            case 0xD000:
            // Draw height * 8 sprite in Xor mode with values at the address stored in the index
                x = (opcode & 0x0F00) >> 8;
                y = (opcode & 0x00F0) >> 4;
                height = opcode & 0x000F;
                index = cpu.getIndex();

                if (height == 0 && gpu.getIsExtended()){
                    SChipSprite.clear();
                    for(int i = 0; i < 32; i += 2){
                        SChipSprite.push_back(cpu.getMemory(index + i) << 8 | cpu.getMemory(index + i + 1));
                    }
                    val =  gpu.SChipDraw(cpu.getReg(x), cpu.getReg(y), SChipSprite);
                    cpu.set(15, val);
                }
                else{
                    sprite.clear();
                    for(int i = 0; i < height; i++){
                        sprite.push_back(cpu.getMemory(index + i));
                    }
                    val =  gpu.draw(cpu.getReg(x), cpu.getReg(y), height, sprite);
                    cpu.set(15, val);
                }

                break;
            
            case 0xE000:
            // Keyboard interrupt checks
                x = (opcode & 0x0F00) >> 8;
                switch(opcode & 0x00FF){
                    case 0x009E:
                        cpu.ifKey(x);
                        break;

                    case 0x00A1:
                        cpu.ifNotKey(x);
                        break;
                    
                    default:
                        break;
                }
                break;
            
            case 0xF000:
                x = (opcode & 0x0F00) >> 8;
                switch(opcode & 0x00FF){
                    case 0x0007:
                    // Get the state of the delay timer
                        cpu.getDelay(x);
                        break;

                    case 0x000A:
                    // Wait for an event to occur
                        val = gpu.waitEvent(&event);
                        cpu.setKey(val, 1);
                        cpu.set(x, val);
                        break;
                    
                    case 0x0015:
                    // Set delay timer to V[x]
                        cpu.setDelay(x);
                        break;
                    
                    case 0x0018:
                    // Set sound timer to V[x]
                        cpu.setSound(x);
                        break;

                    case 0x001E:
                    // Add V[x] to the index
                        cpu.addI(x);
                        break;
                    
                    case 0x0029:
                    // Get the font which number is stored in V[x]
                        cpu.getFont(x);
                        break;
                    
                    case 0x0030:
                    // Get the 16x16 font which number is stored in V[x]
                        cpu.getFont16(x);
                        break;
                    
                    case 0x0033:
                    // Store BCD representation of V[x] at the address stored in the index
                        cpu.BCD(x);
                        break;
                    
                    case 0x0055:
                    // Dump the x+1 registries in memory starting at the address stored in the index
                        cpu.regDump(x);
                        break;
                    
                    case 0x0065:
                    // Load the x+1 registries in memory starting at the address stored in the index
                        cpu.regLoad(x);
                        break;
                    
                    case 0x0075:
                    // Dump the x+1 registries in HP-RPL user flags
                        cpu.userRegDump(x);
                        break;
                    
                    case 0x0085:
                    // Load the x+1 registries in HP-RPL user flags in flags
                        cpu.userRegLoad(x);
                        break;
                    
                    default:
                        break;
                }
                break;

            default:
            // Stop if an unknown opcode occur
                cout << "Unknown opcode" << opcode << endl;
                break;
            
        }
    
    unsigned short res;

    // Check if the is a keyboard interrupt
    res = gpu.pollEvent(&event);

    if(res != 0x00FF){
        cpu.setKey(res & 0x00FF, (res & 0xFF00) >> 8);
    }

    // Decrement delay timer
    if(cpu.getDelayTimer() > 0){
        cpu.updateDelay();
    }

    // Decrement sound timer
    if(cpu.getSoundTimer() > 0)
    {
        if(cpu.getSoundTimer() == 1){
            printf("BEEP!\n");
        }
        cpu.updateSound();
    }  

    // Try to emulate 50 Hz frequency of CPU
   //usleep(2000);
    }
    
    return 0;
}
