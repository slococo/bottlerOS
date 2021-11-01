#include "cpu_id.h"

int cpu_id(int *, int *, int *, int *);

int check_fpu() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return edx & CPUID_FEAT_EDX_FPU;
}

int check_sse() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return edx & CPUID_FEAT_EDX_SSE;
}

int check_sse2() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return edx & CPUID_FEAT_EDX_SSE2;
}

int check_sse41() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_SSE4_1;
}

int check_sse42() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_SSE4_2;
}

int check_avx() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_AVX;
}

int check_fma() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_FMA;
}

int check_f16c() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_F16C;
}

int check_avx2() {
    int eax = 7, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ebx & CPUID_FEAT_AVX2;
}

int check_vaesni() {
    int eax = 7, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ebx & CPUID_FEAT_VAES;
}

int check_vpclmulqdq() {
    int eax = 7, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ebx & CPUID_FEAT_VPCLMULQDQ;
}

int check_pclmulqdq() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_PCLMUL;
}

int check_sse3() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_SSSE3;
}

int check_mmx() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return edx & CPUID_FEAT_EDX_MMX;
}

int check_aes() {
    int eax = 1, ebx = 0, ecx = 0, edx = 0;
    cpu_id(&edx, &ecx, &ebx, &eax);
    return ecx & CPUID_FEAT_ECX_AES;
}

int check_cpuid() {
    return cpu_id_support();
}

static const int len = 16;
int (* checks[])() = {check_cpuid, check_mmx, check_sse, check_sse2, check_sse3, check_sse41, check_sse42, check_aes, check_pclmulqdq, check_avx, check_vaesni, check_vpclmulqdq, check_f16c, check_fma, check_avx2, check_fpu};
char * supports[] = {"cpuid_support", "mx_support", "sse_support", "sse2_support", "sse3_support", "sse41_support", "sse42_support", "aes_support", "pclmulqdq_support", "avx_support", "vaesni_support", "vpclmulqdq_support", "f16c_support", "fma_support", "avx2_support", "fpu_support"};

void cpufeatures(int argc, char *argv[]) {
    for (int i = 0; i < len; i++) {
        if (checks[i]()) {
            printString(supports[i]);
	        newline();
        }
    }
    addEOF();
    sys_exit();
}
