include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=0000-sampleCodeModule.elf
SOURCES=$(wildcard *.c)
SOURCES_SHELL=$(wildcard shell/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_PROMPT=$(wildcard shell/commands/*.c)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHELL=$(SOURCES_SHELL:.c=.o)
OBJECTS_PROMPT=$(SOURCES_PROMPT:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

ALL_OBJECTS= $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_SHELL) $(OBJECTS_COMMANDS) $(OBJECTS_PROMPT)

STATICLIBS=

all: $(MODULE) $(MODULE_ELF) #shellModule

#shellModule:
#	cd shell; make

$(MODULE): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_ASM) $(OBJECTS_PROMPT) -o ../$(MODULE)

$(MODULE_ELF): $(STATICLIBS) $(ALL_OBJECTS)
	$(LD) $(LDFLAGS) -I./include -I./shell/include -T sampleCodeModule.ld --oformat=elf64-x86-64 $(OBJECTS) $(OBJECTS_SHELL) $(OBJECTS_ASM) $(OBJECTS_PROMPT) -o ../$(MODULE_ELF)

%.o:%.c
	$(GCC) $(GCCFLAGS) -T _loader.c -I./include -I./shell/include -c $< -o $@

%.o:%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
#	cd shell; make clean
	rm -rf *.o
	cd shell; rm -rf *.o
	cd shell/commands; rm -rf *.o

.PHONY: all clean print

# include ../Makefile.inc

# MODULE=0000-sampleCodeModule.bin
# SOURCES=$(wildcard [^_]*.c)

# all: $(MODULE)

# $(MODULE): $(SOURCES)
# 	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) -I./include -o ../$(MODULE)

# clean:
# 	rm -rf *.o

# .PHONY: all clean print
