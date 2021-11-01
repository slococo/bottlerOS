include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=0000-sampleCodeModule.elf
SOURCES=$(wildcard *.c)
SOURCES_SHELL=$(wildcard shell/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_PROMPT=$(wildcard shell/commands/*.c)
SOURCES_BOTTLER=$(wildcard bottler/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_PROMPT=$(SOURCES_PROMPT:.c=.o)
OBJECTS_BOTTLER=$(SOURCES_BOTTLER:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

ALL_OBJECTS= $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_SHELL) $(OBJECTS_COMMANDS) $(OBJECTS_PROMPT) $(OBJECTS_BOTTLER)

STATICLIBS=

all: $(MODULE) $(MODULE_ELF)

$(MODULE): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_ASM) $(OBJECTS_PROMPT) $(OBJECTS_BOTTLER) -o ../$(MODULE)

$(MODULE_ELF): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -I./include -I./shell/include -I./bottler/include -T sampleCodeModule.ld --oformat=elf64-x86-64 $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_ASM) $(OBJECTS_PROMPT) $(OBJECTS_BOTTLER) -o ../$(MODULE_ELF)

%.o:%.c
	$(GCC) $(GCCFLAGS) -T _loader.c -I./include -I./shell/include -I./bottler/include -c $< -o $@

%.o:%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(ALL_OBJECTS)

.PHONY: all clean print