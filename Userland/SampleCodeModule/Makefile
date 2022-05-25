#COMIENZO PRUEBA

include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_OTHERS=$(wildcard **/*c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_OTHERS=$(SOURCES_OTHERS:.c=.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) _loader.o
	$(LD)  -T sampleCodeModule.ld _loader.o  $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) -o ../$(MODULE)
	$(LD)  -T sampleCodeModule.ld _loader.o $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) -o ../0000-sampleCodeModule.elf --oformat=elf64-x86-64

_loader.o:_loader.c
	$(GCC) $(GCCFLAGS) -c _loader.c

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c  $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o asm/*.o **/*.o

.PHONY: all clean print
#FIN PRUEBA

#GRACIAS BRITU!!!!

# ARRANCA BRITU
#include ../Makefile.inc
#
#MODULE=0000-sampleCodeModule.bin
#SOURCES=$(wildcard [^_]*.c)
#SOURCES_ASM=$(wildcard asm/*.asm)
#SOURCES_OTHERS=$(wildcard **/*c)
#
#OBJECTS=$(SOURCES:.c=.o)
#OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
#OBJECTS_OTHERS=$(SOURCES_OTHERS:.c=.o)
#
#all: $(MODULE)
#
#
#
#
#$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) _loader.o
#	$(LD) -T sampleCodeModule.ld _loader.o $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_OTHERS) -o ../$(MODULE)
#
#_loader.o:_loader.c
#	$(GCC) $(GCCFLAGS) -c _loader.c
#
#%.o:%.c
#	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@
#
#
#
#%.o:%.asm
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#clean:
#	rm -rf *.o asm/*.o **/*.o
#
#.PHONY: all clean print

# FIN BRITU

#COMIENZO DEL ORIGINAL

#include ../Makefile.inc
#
#MODULE=0000-sampleCodeModule.bin
#SOURCES=$(wildcard [^_]*.c)
#
##copia de lo del kernel
#SOURCES_ASM=$(wildcard asm/*.asm)
#OBJECTS=$(SOURCES:.c=.o)
#OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
#
#all: $(MODULE)
#
#$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM)
#	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)
#	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) -o ../0000-sampleCodeModule.elf
#
#%.o: %.c
#	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@
#
#%.o: %.asm
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#clean:
#	rm -rf *.o
#
#.PHONY: all clean print
#FIN DE ORIGINAL

#include ../Makefile.inc
#
#
#MODULE=0000-sampleCodeModule.bin
##SOURCES=$(wildcard [^_]*.c)
#SOURCES=$(wildcard **/[^_]*.c)
##copia de lo del kernel
#SOURCES_ASM=$(wildcard asm/*.asm)
#OBJECTS=$(SOURCES:.c=.o)
#OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
#
#
#all: $(MODULE)
#
#$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM)
#	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)
#	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) -o ../0000-sampleCodeModule.elf
#
#%.o: %.c
#	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@
#
#%.o: %.asm
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#clean:
#	rm -rf *.o
#
#.PHONY: all clean print

# Intento de hacerlo como en kernel
#MODULE=0000-sampleCodeModule.bin
#SOURCES=$(wildcard [^_]*.c)
##copia de lo del kernel
#SOURCES_ASM=$(wildcard asm/*.asm)
#OBJECTS=$(SOURCES:.c=.o)
#OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
#
#all: $(MODULE)
#
#$(MODULE): $(OBJECTS) $(SOURCES) $(OBJECTS_ASM)
#	$(LD) $(LDFLAGS) -T sampleCodeModule.ld  -o $(MODULE) $(OBJECTS) $(OBJECTS_ASM)
#	$(LD) $(LDFLAGS) -T ./Kernel/kernel.ld --oformat=elf64-x86-64 -o 0000-sampleCodeModule.elf  $(OBJECTS) $(OBJECTS_ASM)
#%.o: %.c
#	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@
#
#%.o : %.asm
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#$(LOADEROBJECT):
#	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)
#
#clean:
#	rm -rf asm/*.o *.o *.bin
#
#.PHONY: all clean print

#El que estaba antes
#include ../Makefile.inc
#
#MODULE=0000-sampleCodeModule.bin
##SOURCES=$(wildcard [^_]*.c)
#SOURCES=$(wildcard **/[^_]*.c)
##copia de lo del kernel
#SOURCES_ASM=$(wildcard asm/*.asm)
#OBJECTS=$(SOURCES:.c=.o)
#OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
#
#all: $(MODULE)
#
#$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM)
#	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)
#	$(GCC) $(GCCFLAGS) -I ./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) -o ../0000-sampleCodeModule.elf
#
#%.o: %.c
#	$(GCC) $(GCCFLAGS) -I ./include -c $< -o $@
#
#%.o: %.asm
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#clean:
#	rm -rf *.o
#
#.PHONY: all clean print
#Otra copia
#$(MODULE): $(SOURCES) $(OBJECTS) $(OBJECTS_ASM)
#	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -o ../$(MODULE)
#	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(SOURCES) -o ../0000-sampleCodeModule.elf
#
#%.o: %.c
#	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@
#
#%.o: %.asm
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#clean:
#	rm -rf *.o
#
#.PHONY: all clean print


#El que pasaron
#include ../Makefile.inc
#
#MODULE=0000-sampleCodeModule.bin
#SOURCES=$(wildcard [^_]*.c)

#SOURCES_ASM=$(wildcard asm/*.asm)
#OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)
#
#all: $(MODULE)
#
#$(MODULE): $(SOURCES) $(OBJECTS_ASM)
#	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../$(MODULE)
#	$(GCC) $(GCCFLAGS) -I./include -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../0000-sampleCodeModule.elf
#
#
#obj/%.asm.o : asm/%.asm
#	mkdir -p obj
#	$(ASM) $(ASMFLAGS) $< -o $@
#
#clean:
#	rm -rf *.o
#	rm -rf obj
#
#.PHONY: all clean print