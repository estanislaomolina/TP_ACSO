; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat
extern strlen
extern strcpy


string_proc_list_create_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     edi, 16
        call    malloc
        mov     QWORD  [rbp-8], rax
        cmp     QWORD  [rbp-8], 0
        jne     .L2
        mov     eax, 0
        jmp     .L3
.L2:
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax], 0
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax+8], 0
        mov     rax, QWORD  [rbp-8]
.L3:
        leave
        ret

string_proc_node_create_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     eax, edi
        mov     QWORD  [rbp-32], rsi
        mov     BYTE  [rbp-20], al
        mov     edi, 32
        call    malloc
        mov     QWORD  [rbp-8], rax
        cmp     QWORD  [rbp-8], 0
        jne     .L5
        mov     eax, 0
        jmp     .L6
.L5:
        mov     rax, QWORD  [rbp-8]
        movzx   edx, BYTE  [rbp-20]
        mov     BYTE  [rax+16], dl
        mov     rax, QWORD  [rbp-8]
        mov     rdx, QWORD  [rbp-32]
        mov     QWORD  [rax+24], rdx
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax], 0
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax+8], 0
        mov     rax, QWORD  [rbp-8]
.L6:
        leave
        ret

string_proc_list_add_node_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        mov     QWORD  [rbp-24], rdi
        mov     eax, esi
        mov     QWORD  [rbp-40], rdx
        mov     BYTE  [rbp-28], al
        movzx   eax, BYTE  [rbp-28]
        mov     rdx, QWORD  [rbp-40]
        mov     rsi, rdx
        mov     edi, eax
        call    string_proc_node_create_asm
        mov     QWORD  [rbp-8], rax
        cmp     QWORD  [rbp-8], 0
        je      .L11
        mov     rax, QWORD  [rbp-24]
        mov     rax, QWORD  [rax+8]
        test    rax, rax
        jne     .L10
        mov     rax, QWORD  [rbp-24]
        mov     rdx, QWORD  [rbp-8]
        mov     QWORD  [rax], rdx
        mov     rax, QWORD  [rbp-24]
        mov     rdx, QWORD  [rbp-8]
        mov     QWORD  [rax+8], rdx
        jmp     .L7
.L10:
        mov     rax, QWORD  [rbp-24]
        mov     rax, QWORD  [rax+8]
        mov     rdx, QWORD  [rbp-8]
        mov     QWORD  [rax], rdx
        mov     rax, QWORD  [rbp-24]
        mov     rdx, QWORD  [rax+8]
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax+8], rdx
        mov     rax, QWORD  [rbp-24]
        mov     rdx, QWORD  [rbp-8]
        mov     QWORD  [rax+8], rdx
        jmp     .L7
.L11:
        nop
.L7:
        leave
        ret

string_proc_list_concat_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     QWORD  [rbp-40], rdi
        mov     eax, esi
        mov     QWORD  [rbp-56], rdx
        mov     BYTE  [rbp-44], al
        cmp     QWORD  [rbp-40], 0
        je      .L13
        cmp     QWORD  [rbp-56], 0
        jne     .L14
.L13:
        mov     eax, 0
        jmp     .L15
.L14:
        mov     rax, QWORD  [rbp-40]
        mov     rax, QWORD  [rax]
        mov     QWORD  [rbp-8], rax
        mov     rax, QWORD  [rbp-56]
        mov     rdi, rax
        call    strlen
        add     rax, 1
        mov     rdi, rax
        call    malloc
        mov     QWORD  [rbp-16], rax
        cmp     QWORD  [rbp-16], 0
        jne     .L16
        mov     eax, 0
        jmp     .L15
.L16:
        mov     rdx, QWORD  [rbp-56]
        mov     rax, QWORD  [rbp-16]
        mov     rsi, rdx
        mov     rdi, rax
        call    strcpy
        jmp     .L17
.L19:
        mov     rax, QWORD  [rbp-8]
        movzx   eax, BYTE  [rax+16]
        cmp     BYTE  [rbp-44], al
        jne     .L18
        mov     rax, QWORD  [rbp-8]
        mov     rdx, QWORD  [rax+24]
        mov     rax, QWORD  [rbp-16]
        mov     rsi, rdx
        mov     rdi, rax
        call    str_concat
        mov     QWORD  [rbp-24], rax
        mov     rax, QWORD  [rbp-16]
        mov     rdi, rax
        call    free
        mov     rax, QWORD  [rbp-24]
        mov     QWORD  [rbp-16], rax
.L18:
        mov     rax, QWORD  [rbp-8]
        mov     rax, QWORD  [rax]
        mov     QWORD  [rbp-8], rax
.L17:
        cmp     QWORD  [rbp-8], 0
        jne     .L19
        mov     rax, QWORD  [rbp-16]
.L15:
        leave
        ret
