all: chip8

chip8: gpu.o cpu.o main.o
	g++ -o chip8 cpu.o gpu.o main.o -lsfml-graphics -lsfml-window -lsfml-system 

cpu.o: cpu.cpp cpu.h
	g++ -o cpu.o -c cpu.cpp -W -Wall -ansi -pedantic

gpu.o: gpu.cpp gpu.h
	g++ -o gpu.o -c gpu.cpp -W -Wall -ansi -pedantic

main.o: main.cpp cpu.h gpu.h
	g++ -o main.o -c main.cpp -W -Wall -ansi -pedantic

clean:
	rm -rf *.o

mrproper: clean
	rm -rf chip8