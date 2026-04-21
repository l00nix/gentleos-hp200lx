;
; Copyright (c) 2019-2026 luke8086.
; Distributed under the terms of GPL-2 License.
;

;
; kernel/boot.s - minimal bootloader for USB disks (no error handling)
;

[org 0x7c00]
[cpu 8086]

TARGET_SEGMENT  equ 0x1000
SECTOR_COUNT    equ 127


    ; Setup segments and stack
    cli
    mov ax, TARGET_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xffff
    sti

    ; Preserve disk number
    mov [cs:disk], dl

    ; Print intro text
    mov word [cs:puts_str], str_intro
    call puts

    ; Retrieve sectors per track
    push es
    mov ah, 0x08
    mov dl, [cs:disk]
    int 0x13
    and cl, 0x3f
    mov [cs:spt], cl
    pop es

    ; Read data from floppy, one sector at a time, ignoring errors
    mov si, SECTOR_COUNT
    xor bx, bx
    xor ch, ch              ; Cylinder 0
    mov dl, [cs:disk]       ; Disk number
    xor dh, dh              ; Head 0
    mov cl, 1               ; Start sector (1-indexed)

.read_loop:
    ; Read one sector
    mov ax, 0x0201
    int 0x13

    ; Print progress dot
    mov word [cs:puts_str], str_dot
    call puts

    ; Advance target buffer
    add bx, 512

    ; Advance sector number
    inc cl
    cmp cl, [cs:spt]
    jbe .read_next

    ; Wrap to sector 1 and flip head
    mov cl, 1
    xor dh, 1

    ; If head is 0, advance cylinder
    jnz .read_next
    inc ch

.read_next:
    dec si
    jnz .read_loop

    ; Print outro text
    mov word [cs:puts_str], str_outro
    call puts

    ; Move the loaded data back by 0x300 bytes, so that kernel starts at 0x100
    mov si, 0x0300
    mov di, 0x0000
    mov cx, SECTOR_COUNT * (512 / 2)
    cld
    rep movsw

    ; Save a known value to the beginning of the segment
    mov [0], word 0xcafe

    ; Jump to the COM file
    jmp TARGET_SEGMENT:0x100


; Print a string
puts:
    call save_regs

    mov si, [cs:puts_str]

.puts_loop:
    mov al, [cs:si]
    or al, al
    jz .puts_done

    mov ah, 0x0e
    xor bx, bx
    int 0x10

    inc si
    jmp .puts_loop
.puts_done:

    call restore_regs
    ret

; String to print
puts_str:
    dw 0

; Save preserved registers
save_regs:
    mov [cs:saved_regs + 0], ax
    mov [cs:saved_regs + 2], bx
    mov [cs:saved_regs + 4], cx
    mov [cs:saved_regs + 6], dx
    mov [cs:saved_regs + 8], bp
    mov [cs:saved_regs + 10], si
    mov [cs:saved_regs + 12], di
    ret

; Restore preserved registers
restore_regs:
    mov ax, [cs:saved_regs + 0]
    mov bx, [cs:saved_regs + 2]
    mov cx, [cs:saved_regs + 4]
    mov dx, [cs:saved_regs + 6]
    mov bp, [cs:saved_regs + 8]
    mov si, [cs:saved_regs + 10]
    mov di, [cs:saved_regs + 12]
    ret

; Preserved registers
saved_regs:
    dw 0 ; AX
    dw 0 ; BX
    dw 0 ; CX
    dw 0 ; DX
    dw 0 ; BP
    dw 0 ; SI
    dw 0 ; DI

; Sectors per track (default 18, auto-detected at boot)
spt: db 18

; Disk number
disk: db 0

; Intro text
str_intro: db 0x0d, 0x0a, "Booting GentleOS [github.com/luke8086/gentleos]...", 0

; Progress dot
str_dot: db '.', 0

; Outro text
str_outro: db "ok", 0

; MBR partition table with a single bootable partition
times 0x1be - ($ - $$) db 0
db 0x80, 0x00, 0x02, 0x00
db 0x01, 0x00, 0x3f, 0x00
dd 0x01, 0x7f

; Boot-loader designator
times 0x1fe - ($ - $$) db 0
dw 0b10101010_01010101
