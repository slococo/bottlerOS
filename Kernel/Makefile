include Makefile.inc

KERNEL=kernel.bin
KERNEL_ELF=kernel.elf
SOURCES=$(wildcard *.c)
SOURCES_INTERRUPTIONS=$(wildcard interruptions/*.c)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_INTERRUPTIONS_ASM=$(wildcard interruptions/*.asm)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_INTERRUPTIONS=$(SOURCES_INTERRUPTIONS:.c=.o)
OBJECTS_INTERRUPTIONS_ASM=$(SOURCES_INTERRUPTIONS_ASM:.asm=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

ALL_OBJECTS= $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_INTERRUPTIONS_ASM) $(OBJECTS_DRIVERS)

all: $(KERNEL) $(KERNEL_ELF)

$(KERNEL): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^
	
$(KERNEL_ELF): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $@ $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I./drivers -I./interruptions -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf $(ALL_OBJECTS) *.bin

.PHONY: all clean
