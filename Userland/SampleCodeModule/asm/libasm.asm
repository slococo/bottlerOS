GLOBAL sys_read, sys_write, sys_time, quadSolver
GLOBAL _getMem, sys_loadProcess
GLOBAL raiseOpcodeExc
GLOBAL _getRegs, sys_switchContext
GLOBAL cpu_id, cpu_id_support

section .text

sys_write:
    push rbp
    mov rbp, rsp

	push rdi
	push rsi
	push rdx
	push rcx

	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 0
	int 80h

	pop rcx
	pop rdx
	pop rsi
	pop rdi

    mov rsp, rbp
    pop rbp
    ret

sys_read:
    push rbp
    mov rbp, rsp

	push rdi
	push rsi
	push rdx
	push rcx

	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 1
	int 80h

	pop rcx
	pop rdx
	pop rsi
	pop rdi

    mov rsp, rbp
    pop rbp
    ret

_getMem:
	push rbp
	mov rbp, rsp

	mov eax, dword [rdi]

	mov rsp, rbp
	pop rbp
	ret

_getRegs:
	mov [regs], rax
	mov [regs + 8], rbx
	mov [regs + 16], rcx
	mov [regs + 24], rdx
	mov [regs + 32], rbp
	mov [regs + 40], rdi
	mov [regs + 48], rsi
	mov [regs + 56], r8
	mov [regs + 64], r9
	mov [regs + 72], r10
	mov [regs + 80], r11
	mov [regs + 88], r12
	mov [regs + 96], r13
	mov [regs + 104], r14
	mov [regs + 112], r15

	lea rax, [rsp + 8]
	mov [regs + 120], rax

	mov rax, [rsp]
	mov [regs + 128], rax

	mov rax, regs
	ret

cpu_id:
	push rbp
	mov rbp, rsp

	push rax
	mov rax, rdi
	mov qword [auxRDI], rax
	mov rax, rsi
	mov qword [auxRSI], rax
	mov rax, rdx
	mov qword [auxRDX], rax
	mov rax, rcx
	mov qword [auxRCX], rax

	mov eax, [rcx]
	mov ecx, [rdx]
	cpuid

	push r12
	mov r12, 0
	mov r12, qword [auxRDI]
	mov [r12], edx
	mov r12, qword [auxRSI]
	mov [r12], ecx
	mov r12, qword [auxRDX]
	mov [r12], ebx
	mov r12, qword [auxRCX]
	mov [r12], eax
	pop r12

	pop rax

	mov rsp, rbp
	pop rbp
	ret

cpu_id_support:
    pushfq                               ;Save EFLAGS
    pushfq                               ;Store EFLAGS
    xor dword [rsp], 0x00200000           ;Invert the ID bit in stored EFLAGS
    popfq                                ;Load stored EFLAGS (with ID bit inverted)
    pushfq                               ;Store EFLAGS again (ID bit may or may not be inverted)
    pop rax                              ;eax = modified EFLAGS (ID bit may or may not be inverted)
    xor rax, [rsp]                        ;eax = whichever bits were changed
    popfq                                ;Restore original EFLAGS
    and rax, 0x00200000                   ;eax = zero if ID bit can't be changed, else non-zero
    ret

raiseOpcodeExc:
	ud2

sys_loadProcess:
	push rbp
    mov rbp, rsp

	push rdi
	push rsi

	mov rsi, rdi
	mov rdi, 3
    int 80h

	pop rsi
	pop rdi

    mov rsp, rbp
    pop rbp
    ret

sys_switchContext:
	push rbp
    mov rbp, rsp

    int 81h

    mov rsp, rbp
    pop rbp
    ret

sys_time:
	push rbp
    mov rbp, rsp

	push rdi
	push rsi
	push rdx
	push rcx

	mov rsi, rdi ; option
    mov rdi, 2
    int 80h

	pop rcx
	pop rdx
	pop rsi
	pop rdi

    mov rsp, rbp
    pop rbp
    ret

quadSolver:
	push rbp
    mov rbp, rsp

    mov word [number4], -4
	fild word [number4]
    movsd qword [varA], xmm0
	fld qword [varA] ; a
	fmulp ; st(1) * st(0) y deja en st(1), ademas popea (saca st(0))
    movsd qword [varC], xmm2
	fld qword [varC] ; xmm2
	fmulp ; ahora tengo -4ac

    movsd qword [varB], xmm1
	fld qword [varB] ; b
	fld qword [varB] ; b
	fmulp ; hago b**2 y dejo en st(0)
	faddp

    fstp qword [resAux]
    mov word [number0], 0
	fild word [number0]
    fld qword [resAux]
	fcomi st0, st1
    jc .notReal ; si 0 > st(1)
    faddp

	fsqrt
    mov qword [resAux], 0
	fstp qword [resAux] ; guardo la raíz
    
	fld qword [varA]
    mov word [number2], 2
	fild qword [number2]
	fmulp
    mov qword [number1], 1
    fild qword [number1]
	fdivrp ; divide 1/2a
	fst qword [divAux] ; guardo el 1/2a
	fld qword [varB]
	fld qword [resAux]
	fsubrp
	fmulp
    fstp qword [resOne]
    movsd xmm4, qword [resOne]
    movsd qword [rdi], xmm4 ; guardamos primera solucion

    fld qword [varB]
    fld qword [resAux]
    fchs
    fsubrp
    fmul qword [divAux]
    fstp qword [resTwo]
    movsd xmm5, qword [resTwo]
    movsd qword [rsi], xmm5 ; guardamos primera solucion

	mov rax, 1
	jmp .quit

.notReal:
	mov rax, 0

.quit:
    mov rsp, rbp
    pop rbp
    ret	

section .bss
	number4 resb 8
	number0 resb 8
	number1 resb 8
	number2 resb 8
    number5 resb 8
    varA resb 8
    varB resb 8
    varC resb 8
    resOne resb 8
    resTwo resb 8
    resAux resb 8
    divAux resb 8
	regs resb 120 ; 8 bytes * 16 regs
	auxRDI resb 8
	auxRSI resb 8
	auxRDX resb 8
	auxRCX resb 8