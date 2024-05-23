FINAL = all
COMPILER = gcc
ASSEMBLER = nasm
CFLAGS = -std=c99 -c -m32
SFLAGS = -f elf32
OFLAGS = -no-pie -m32

$(FINAL) : main.o func.o
	$(COMPILER) main.o func.o $(OFLAGS) -o $(FINAL) -lm
main.o : main.c
	$(COMPILER) $(CFLAGS) -o main.o main.c
func.o : func.asm
	$(ASSEMBLER) $(SFLAGS) -o func.o func.asm
clean :
	rm $(FINAL) *.o
