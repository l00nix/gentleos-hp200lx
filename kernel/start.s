; ---------------------------------------------------------------------------------------
; Copyright (c) 2019-2026 luke8086
; Distributed under the terms of GPL-2 License
; ---------------------------------------------------------------------------------------
; File: start.s - startup code
; ---------------------------------------------------------------------------------------

[cpu 8086]

extern _krn_main

section _TEXT class=CODE

resb 0x100
..start:
    ; save program DGROUP for Turbo C interrupt functions
    mov [DGROUP@], ds

    ; jump to the C code
    jmp _krn_main

section _DATA class=DATA

global _krn_data_seg
_krn_data_seg:

global DGROUP@
DGROUP@:
    dw 0

section _DATAEND class=DATAEND

global _krn_marker_data_end
_krn_marker_data_end:
    dq 0xf0cacc1a

section _BSS class=BSS
section _BSSEND class=BSSEND

group DGROUP _TEXT _DATA _DATAEND _BSS _BSSEND
