GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _systemCallsHandler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN systemCallsDispatcher
; EXTERN preserveStack
; EXTERN newStack
EXTERN changeWindow
EXTERN updateRSP
EXTERN nextProcess

GLOBAL switchContext
GLOBAL loadProcess
GLOBAL _initialize_stack_frame
;GLOBAL _switchContext, forceTimerAux

EXTERN getFPUaddress, getSSEaddress
EXTERN checkSleeping

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState
	fsave [bytesForFPU]
	fxsave [bytesForSSEAligned]

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	fxrstor [bytesForSSEAligned]
	frstor [bytesForFPU]
	popState
	iretq
%endmacro


%macro exceptionHandler 1
	mov [insPointer], rsp
	push rax
	lea rax, [rsp + 4 * 8]
	mov [rspPointer], rax
	pop rax

	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, [insPointer]
	mov rdx, [rspPointer]
	mov rcx, rsp
	call exceptionDispatcher

	popState

	iretq
%endmacro

%macro pushStateNoRax 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popStateNoRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret

_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

;8254 Timer (Timer Tick)
_irq00Handler:
	pushState

	mov rsi, rsp
	and rsp, -16
	sub rsp, 108
	fsave [rsp]
	and rsp, -16
	sub rsp, 512
	fxsave [rsp]
	push rsi

	call checkSleeping

	; mov rdi, rsp
	; call updateRSP

	mov rdi, 0
	call irqDispatcher

	mov rdi, rsp
	call nextProcess
	mov rsp, rax

	mov al, 20h
	out 20h, al

	pop rsp
	mov rax, rsp
	and rsp, -16
	sub rsp, 108
	frstor [rsp]
	and rsp, -16
	sub rsp, 512
	fxrstor [rsp]
	mov rsp, rax

	popState
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid OPCODE Exc
_exception6Handler:
	exceptionHandler 6

haltcpu:
	sti
	hlt
	cli
	ret

_initialize_stack_frame:
    mov r10, rsp
    mov rsp, rsi
    
	push 0x0 ; ss
    push rsi ; sp
    push 0x202 ; rflags
    push 0x08 ; cs
    push rdi ; IP

	mov rsi, rcx
	mov rdi, rdx 

    pushState

	mov rsi, rsp
	and rsp, -16
	sub rsp, 108
	fsave [rsp]
	and rsp, -16
	sub rsp, 512
	fxsave [rsp]
	push rsi
	mov rax, rsp

    mov rsp, r10
    ret

; System calls (int 80h)
_systemCallsHandler:
	pushStateNoRax
	; fsave [bytesForFPU]
	; fxsave [bytesForSSEAligned]

	mov [auxRSI], rsi
	mov rsi, rsp
	and rsp, -16
	sub rsp, 108
	fsave [rsp]
	; and rsp, -16
	; sub rsp, 512
	; fxsave [rsp]
	push rsi
	mov rsi, [auxRSI]

	call systemCallsDispatcher

	; fxrstor [bytesForSSEAligned]
	; frstor [bytesForFPU]

	pop rsp
	mov [auxRAX], rax
	mov rax, rsp
	and rsp, -16
	sub rsp, 108
	frstor [rsp]
	; and rsp, -16
	; sub rsp, 512
	; fxrstor [rsp]
	mov rsp, rax
	mov rax, [auxRAX]

	popStateNoRax
	iretq

SECTION .data
	align 16
	bytesForSSEAligned times 512 db 0

SECTION .bss
	aux resq 1
	bytesForSSE resb 512
	bytesForFPU resb 108
	insPointer resb 8
	rspPointer resb 8
	auxRSI resb 8
	auxRDI resb 8
	auxRAX resb 8