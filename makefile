#because linking is faster then compiling
#linking
lc3vm: memory.o cpu.o main.o
	gcc memory.o cpu.o main.o -o lc3vm

#compiling
memory.o: src/memory.c
	gcc -c src/memory.c
cpu.o: src/cpu.c
	gcc -c src/cpu.c 
main.o: src/main.c
	gcc -c src/main.c 
	
clean:
	rm *.o
