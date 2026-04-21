;
; Copyright (c) 2019-2026 luke8086.
; Distributed under the terms of GPL-2 License.
;

;
; kernel/boot.s - Stage 1 bootloader
;

[org 0x7c00]
[cpu 8086]

TARGET_SEGMENT      equ 0x2000
TARGET_OFFSET       equ 0x100
DISK_NO_OFS         equ 0
BOOT2_START         equ 3
BOOT2_SIZE          equ 4
LOAD_RETRY_COUNT    equ 3

    ; Setup segments and stack
    cli
    mov ax, TARGET_SEGMENT
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xffff
    sti

    ; Preserve disk number
    mov [ds:DISK_NO_OFS], dl

    ; Load stage 2 loader
    mov di, LOAD_RETRY_COUNT
.load_stage2:

    ; Reset disk
    xor ah, ah
    int 0x13

    ; Load from disk
    mov ah, 0x02
    mov al, BOOT2_SIZE
    mov bx, TARGET_OFFSET
    mov dl, [ds:DISK_NO_OFS]
    xor dh, dh                  ; Head 0
    xor ch, ch                  ; Cylinder 0
    mov cl, BOOT2_START         ; Start sector (1-indexed)
    int 0x13

    ; Load succeeded
    jnc .load_success

    ; Retry LOAD_RETRY_COUNT times
    dec di
    jnz .load_stage2
    jmp .load_error

    ; Jump to the stage 2 COM file
.load_success:
    jmp TARGET_SEGMENT:0x100

    ; All retries failed
.load_error:
    mov ah, 0x0e
    mov al, 'E'
    xor bx, bx
    int 0x10
.halt:
    hlt
    jmp .halt

; MBR partition table with a single bootable partition
times 0x1be - ($ - $$) db 0
db 0x80, 0x00, 0x02, 0x00
db 0x01, 0x00, 0x3f, 0x00
dd 0x01, 0x7f

; Boot-loader designator
times 0x1fe - ($ - $$) db 0
dw 0b10101010_01010101
