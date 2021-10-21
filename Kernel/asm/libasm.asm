GLOBAL getCharInterrupt, getTimeGen, getMemGen, getRegs, getRSP, forceTimer, enter_region, leave_region

section .text

forceTimer:
	int 20h
	ret

; tiene que ser uint32_t el parametro, no soporta más xchg. aunq también podría ser menos
enter_region:
	;push eax no haría falta pues lo podemos perder...
	mov eax, 1
	xchg eax, [rdi]
	cmp eax, 0
	jne enter_region
	ret

leave_region:
	mov dword [rdi], 0
	ret

getCharInterrupt:
	push rbp
	mov rbp, rsp

	in al, 60h

	mov rsp, rbp
	pop rbp
	ret

getTimeGen:
	push rbp
	mov rbp, rsp

	xor rax, rax
	mov rax, rdi
	out 70h, al
	in al, 71h
	call bcdToDecimal

	mov rsp, rbp
	pop rbp
	ret

bcdToDecimal:
	push rbx
	mov rbx, 0
	mov bl, al
	shr al, 4
	mov bh, 10
	mul bh
	and bl, 0fh
	add al, bl
	pop rbx
	ret

getMemGen:
	push rbp
	mov rbp, rsp

	mov eax, dword [rdi]

	mov rsp, rbp
	pop rbp
	ret

getRSP:
	mov rax, rsp
	ret

getRegs:
	push rbp
	mov rbp, rsp

	mov [regs], rax
	mov [regs + 8], rbx
	mov [regs + 16], rcx
	mov [regs + 24], rdx
	mov [regs + 32], rsp
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

	mov rax, regs

	mov rsp, rbp
	pop rbp
	ret

section .bss
	regs resb 120 ; 8 bytes * 16 regs