[bits 32]
[section .csdfs]
csdfs_superblock: ; the superblock for CSDFS (Compact System Disk FS)
        magic: db 0xc5, 0xdf, 0x50, 0xac ; magic number
        vol_label: db "FarOS Boot Disk " ; volume label
        vol_id: dq 0x1dc5926a300e4af3 ; volume id
        fs_start: dw 0x10 ; LBA where the fs actually starts
        fs_size: dd (2880 - 0x10) ; length of disk in sectors
        media_type: db 0xa3 ; a3 means 3¼" HD 1.44M floppy diskette

        times 63-($-csdfs_superblock) db 0 ; pad to the 63rd byte - end of superblock

        sig: nop ; the signature at the end

[section .text]
;
; Code
;
[extern main]
kernel_entry:
        mov ebx, protected ; log a message
        call print_32
        
        call clear_scr
        call main

        cli
        hlt

[global clear_scr]
clear_scr: ; clear screen
        pushad
        mov ax, 0x0700
        mov ecx, (80 * 25)
        mov edi, 0xb8000
        cld
        rep stosw
        popad
        ret

[global scroll_scr]
scroll_scr:
        pushad
        mov esi, 0xb8000 + (80 * 2)
        mov edi, 0xb8000
        mov ecx, (80 * 24 / 2)
        cld
        rep movsd

        mov ax, 0x0700
        mov ecx, 80
        rep stosw
        popad
        ret

print_32:
        pushad ; pusha but 32bit this time

        ; location of vram
        mov edi, 0xb8000 + (80 * 24 * 2) ; the last row of the 80x25 display
        xor ecx, ecx ; blank out counter
  char_32:
        mov al, [ebx + ecx] ; move what is in the string
        mov ah, 0x4a ; dark red back green fore
        mov [edi + 2 * ecx], ax ; place our character package (styles + character) in the vram

        inc ecx ; move along our string and vram

        cmp byte[ebx + ecx], 0x0 ; is character null?
        jnz char_32 ; if not, continue string
        popad ; popa but 32bit
        ret

protected:
        db "Successfully moved into Protected Mode!",0

        times (512)-($-$$) db 0 ; pad to the 512th byte - end of csdfs extended boot

%macro eh_macro 1 ; exception handler
global eh_%1
eh_%1:
  push dword 0     ; no error code: so we use 0
  push dword %1    ; push interrupt number
  jmp  generic_eh  ; generic eh
%endmacro

%macro e_eh_macro 1 ; exception handler w/ error code
global eh_%1
eh_%1:
  push dword %1    ; push interrupt number
  jmp  generic_eh  ; generic eh
%endmacro

[extern eh_c]
generic_eh:
        pushad ; save registers
        push dword[esp + 0x20]
;        cli
;        hlt
        call eh_c ; our handler
        pop edx   ; help 
        popad ; restore register

        add esp, 8 ; restore our stack: we pushed the error code and interrupt number
        iret       ; adios

eh_macro    0
eh_macro    1
eh_macro    2
eh_macro    3
eh_macro    4
eh_macro    5
eh_macro    6
eh_macro    7
e_eh_macro  8
eh_macro    9
e_eh_macro 10
e_eh_macro 11
e_eh_macro 12
e_eh_macro 13
e_eh_macro 14
eh_macro   15
eh_macro   16
e_eh_macro 17
eh_macro   18
eh_macro   19
eh_macro   20
eh_macro   21
eh_macro   22
eh_macro   23
eh_macro   24
eh_macro   25
eh_macro   26
eh_macro   27
eh_macro   28
eh_macro   29
e_eh_macro 30
eh_macro   31
eh_macro   32
eh_macro   33
eh_macro   34
eh_macro   35
eh_macro   36
eh_macro   37
eh_macro   38
eh_macro   39
eh_macro   40
eh_macro   41
eh_macro   42
eh_macro   43
eh_macro   44
eh_macro   45
eh_macro   46
eh_macro   47

global eh_list ; each one of the macros above
eh_list:
%assign i 0
%rep    48
    dd eh_%+i ; me when the the
%assign i i+1
%endrep
