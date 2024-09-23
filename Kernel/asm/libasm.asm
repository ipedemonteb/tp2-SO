GLOBAL cpuVendor
GLOBAL getKey
GLOBAL getKeyboardStatus
GLOBAL getKeyboardKey
GLOBAL outb
GLOBAL inb
GLOBAL getCurrentTime
GLOBAL setTimerTick

section .text
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid

	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

getKey: 
	push rbp
	mov rbp, rsp
	mov rax, 0

loop: 
	in al, 0x64
	;mov cl, al
	and al, 0x01
	cmp al, 0
	je loop
	in al, 0x60
	mov rsp, rbp
	pop rbp
	ret

;===============================================================================
; getKeyboardStatus - returns keyboard status. 1 if it is active, 
; 0 if not
;===============================================================================
getKeyboardStatus:
    push rbp
    mov rbp, rsp

    xor rax, rax
    in al, 0x64
    and rax, 0x01

    mov rsp, rbp
    pop rbp
    ret

;===============================================================================
; getKeyboardKey - returns the scancode of the pressed key
;===============================================================================
getKeyboardKey:
    push rbp
    mov rbp, rsp

    xor rax, rax
    in al, 0x60
    
    mov rsp, rbp
    pop rbp
    ret

;===============================================================================
; outb - sends "Y" value to I/O port number "X"
;===============================================================================
; arguments:
;	rdi: X - port number
;	rsi: Y - value to send
;===============================================================================
outb:
	push rbp
	mov rbp, rsp

	mov rdx, rdi
	mov rax, rsi
	out dx, al

	mov rsp, rbp
	pop rbp
	ret

;===============================================================================
; inb - returns the value of I/O port "X"
;===============================================================================
; arguments:
;	rdi: X - port number
;===============================================================================
inb:
	push rbp
	mov rbp, rsp

	mov rdx, rsi
	in al, dx

	mov rsp, rbp
	pop rbp
	ret

;===============================================================================
; getCurrentTime - returns current time value according to chosen parameter
; (0 equals seconds, 1 equals minutes, 2 equals hour)
;===============================================================================
; arguments:
;	rdi: chosen parameter
;===============================================================================
getCurrentTime:
	push rbp
	mov rbp, rsp
	xor rax, rax
	mov rax, rdi
	add rax, rdi
	out 70h, al
	in al, 71h
	mov rsp, rbp
	pop rbp
	ret

;===============================================================================
; setTimerTick - sets time between ticks based on parameter "N"
;===============================================================================
; Argumentos:
;	rdi: "N", which equals timertick's frequence in Hz
;===============================================================================
; wiki.osdev.org
setTimerTick:
    push rbp
	mov rbp, rsp

    ; Do some checking
	mov ebx, edi
    mov eax,0x10000                   ; eax = reload value for slowest possible frequency (65536)
    cmp ebx,18                        ; Is the requested frequency too low?
    jbe .gotReloadValue               ; yes, use slowest possible frequency

    mov eax,1                         ; ax = reload value for fastest possible frequency (1)
    cmp ebx,1193181                   ; Is the requested frequency too high?
    jae .gotReloadValue               ; yes, use fastest possible frequency

    ; Calculate the reload value

    mov eax,3579545
    mov edx,0                         ; edx:eax = 3579545
    div ebx                           ; eax = 3579545 / frequency, edx = remainder
    cmp edx,3579545 / 2               ; Is the remainder more than half?
    jb .l1                            ; no, round down
    inc eax                           ; yes, round up
 .l1:
    mov ebx,3
    mov edx,0                         ; edx:eax = 3579545 * 256 / frequency
    div ebx                           ; eax = (3579545 * 256 / 3 * 256) / frequency
    cmp edx,3 / 2                     ; Is the remainder more than half?
    jb .l2                            ; no, round down
    inc eax                           ; yes, round up
 .l2:

 ; Store the reload value and calculate the actual frequency
 .gotReloadValue:
    push rax                          ; Store reload_value for later
    mov [PIT_reload_value],ax         ; Store the reload value for later
    mov ebx,eax                       ; ebx = reload value

    mov eax,3579545
    mov edx,0                         ; edx:eax = 3579545
    div ebx                           ; eax = 3579545 / reload_value, edx = remainder
    cmp edx,3579545 / 2               ; Is the remainder more than half?
    jb .l3                            ; no, round down
    inc eax                           ; yes, round up
 .l3:
    mov ebx,3
    mov edx,0                         ; edx:eax = 3579545 / reload_value
    div ebx                           ; eax = (3579545 / 3) / frequency
    cmp edx,3 / 2                     ; Is the remainder more than half?
    jb .l4                            ; no, round down
    inc eax                           ; yes, round up
 .l4:
    mov [IRQ0_frequency],eax          ; Store the actual frequency for displaying later

 ; Program the PIT channel
    cli                               ; Disabled interrupts (just in case)

    mov al,00110100b                  ; channel 0, lobyte/hibyte, rate generator
    out 0x43, al

    mov ax,[PIT_reload_value]         ; ax = 16 bit reload value
    out 0x40,al                       ; Set low byte of PIT reload value
    mov al,ah                         ; ax = high 8 bits of reload value
    out 0x40,al                       ; Set high byte of PIT reload value

	sti

	mov rsp, rbp
	pop rbp
    ret

section .bss
    IRQ0_frequency:          resd 1          ; Actual frequency of PIT
    PIT_reload_value:        resw 1          ; Current PIT reload value