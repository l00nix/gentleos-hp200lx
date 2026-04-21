; ---------------------------------------------------------------------------------------
; Copyright (c) 2026 luke8086
; Distributed under the terms of GPL-2 License
; ---------------------------------------------------------------------------------------
; File: start.s - Stage 2 bootloader asm code
; ---------------------------------------------------------------------------------------

[cpu 8086]

extern _main

section _TEXT class=CODE

resb 0x100
..start:
    jmp _main


global _intr
_intr:
    push bp
    mov bp, sp

    push si
    push di
    push bp
    push ds
    push es

    mov ax, [bp+4]
    mov byte [intr_int+1], al

    mov si, [bp+6]

    push word [si+16]
    push word [si+18]

    mov ax, [si+0]
    mov bx, [si+2]
    mov cx, [si+4]
    mov dx, [si+6]
    mov bp, [si+8]
    mov di, [si+10]
    mov si, [si+12]

    pop es
    pop ds

intr_int:
    int 0

    mov [cs:intr_saved_ds], ds
    mov [cs:intr_saved_es], es

    pop es
    pop ds
    pop bp

    mov si, [bp+6]
    mov [si+0], ax
    mov [si+2], bx
    mov [si+4], cx
    mov [si+6], dx

    pushf
    pop word [si+14]

    mov ax, [cs:intr_saved_ds]
    mov [si+16], ax
    mov ax, [cs:intr_saved_es]
    mov [si+18], ax

    pop di
    pop si

    pop bp
    ret

intr_saved_ds: dw 0
intr_saved_es: dw 0


global _start_kernel
_start_kernel:
    cli

    mov ax, 0x1000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xffff

    sti
    jmp 0x1000:0x100


global _halt
_halt:
    cli
    hlt
    jmp _halt

section _DATA class=DATA
section _DATAEND class=DATAEND
section _BSS class=BSS
section _BSSEND class=BSSEND

group DGROUP _TEXT _DATA _DATAEND _BSS _BSSEND
