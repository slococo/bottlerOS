
all:  bootloader kernel userland image
buddy:  bootloader kernelBuddy userland image

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all
kernelBuddy:
	cd Kernel; make all MFLAG=-DBUDDY

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean
