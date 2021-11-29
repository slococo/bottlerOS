all:  bootloader kernel userland image
spanish:  bootloader kernelSpanish userland imageSpanish
buddy:  bootloader kernelBuddy userland imageBuddy
free:  bootloader kernelFree userland imageFree

bootloader:
	cd Bootloader; make all

kernel:
	@if ! grep -q "ke" Kernel/flags.txt; then\
		echo "ke" > Kernel/flags.txt;\
    fi
	cd Kernel; make all
kernelSpanish:
	@if ! grep -q "ks" Kernel/flags.txt; then\
		echo "ks" > Kernel/flags.txt;\
    fi
	cd Kernel; make all KFLAG=-DSPANISH
kernelBuddy:
	@if ! grep -q "be" Kernel/flags.txt; then\
		echo "be" > Kernel/flags.txt;\
    fi
	cd Kernel; make all MFLAG=-DBUDDY
kernelFree:
	@if ! grep -q "fr" Kernel/flags.txt; then\
		echo "fr" > Kernel/flags.txt;\
    fi
	cd Kernel; make all FFLAG=-DFREE_EXIT

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all
imageSpanish: kernelSpanish bootloader userland
	cd Image; make all
imageBuddy: kernelBuddy bootloader userland
	cd Image; make all
imageFree: kernelFree bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

test:
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log
	cppcheck --quiet --enable=all --force --inconclusive .

.PHONY: bootloader image imageSpanish imageBuddy imageFree collections kernel kernelSpanish kernelBuddy kernelFree userland all clean test