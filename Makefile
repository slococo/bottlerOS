all:  bootloader kernel userland image
spanish:  bootloader kernelSpanish userland imageSpanish
buddy:  bootloader kernelBuddy userland imageBuddy

bootloader:
	cd Bootloader; make all

kernel:
	@if ! grep -q "ke" flags.txt; then\
		echo "ke" > flags.txt;\
    fi
	cd Kernel; make all
kernelSpanish:
	@if ! grep -q "ks" flags.txt; then\
		echo "ks" > flags.txt;\
    fi
	cd Kernel; make all KFLAG=-DSPANISH
kernelBuddy:
	@if ! grep -q "be" flags.txt; then\
		echo "be" > flags.txt;\
    fi
	cd Kernel; make all MFLAG=-DBUDDY

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all
imageSpanish: kernelSpanish bootloader userland
	cd Image; make all
imageBuddy: kernelBuddy bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image imageSpanish imageBuddy collections kernel kernelSpanish kernelBuddy userland all clean 
