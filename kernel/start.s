; ---------------------------------------------------------------------------------------
; Copyright (c) 2019-2026 luke8086
; Distributed under the terms of GPL-2 License
; ---------------------------------------------------------------------------------------
; File: start.s - startup code
; ---------------------------------------------------------------------------------------

[cpu 8086]

extern krn_main

section _TEXT class=CODE

resb 0x100
..start:
    ; save program DGROUP for Turbo C interrupt functions
    mov [DGROUP@], ds

    ; jump to the C code
    jmp krn_main

section _DATA class=DATA

global krn_data_seg
krn_data_seg:
global DGROUP@
DGROUP@:
    dw 0

section _BSS class=BSS
section _BSSEND class=BSSEND

; free memory begins at the end of BSS
global heap_start
heap_start:

group DGROUP _TEXT _DATA _BSS _BSSEND
