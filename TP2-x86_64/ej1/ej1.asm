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
        ; Crea una lista de procesos de cadena.
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     edi, 16              ; Solicita 16 bytes para la lista
        call    malloc
        mov     QWORD  [rbp-8], rax  ; Guarda el puntero devuelto
        cmp     QWORD  [rbp-8], 0    ; Comprueba si malloc falló
        jne     .L2
        mov     eax, 0              ; Retorna NULL en caso de error
        jmp     .L3
.L2:
        ; Inicializa los campos de la lista (punteros a nodo y final).
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax], 0
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax+8], 0
        mov     rax, QWORD  [rbp-8]
.L3:
        leave
        ret

string_proc_node_create_asm:
        ; Crea un nodo para la lista y asigna valores.
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     eax, edi            ; Tipo del nodo en eax
        mov     QWORD  [rbp-32], rsi ; Guarda el puntero a la cadena
        mov     BYTE  [rbp-20], al   ; Guarda el tipo en el stack
        mov     edi, 32             ; Solicita 32 bytes para el nodo
        call    malloc
        mov     QWORD  [rbp-8], rax  ; Guarda el puntero del nuevo nodo
        cmp     QWORD  [rbp-8], 0
        jne     .L5
        mov     eax, 0              ; Retorna NULL en caso de error
        jmp     .L6
.L5:
        ; Inicializa el nodo: guarda el tipo y la cadena.
        mov     rax, QWORD  [rbp-8]
        movzx   edx, BYTE  [rbp-20]
        mov     BYTE  [rax+16], dl
        mov     rax, QWORD  [rbp-8]
        mov     rdx, QWORD  [rbp-32]
        mov     QWORD  [rax+24], rdx
        ; Inicializa punteros internos del nodo.
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax], 0
        mov     rax, QWORD  [rbp-8]
        mov     QWORD  [rax+8], 0
        mov     rax, QWORD  [rbp-8]
.L6:
        leave
        ret

string_proc_list_add_node_asm:
        ; Agrega un nodo a la lista de procesamiento de cadenas.
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        mov     QWORD  [rbp-24], rdi  ; Guarda puntero a la lista
        mov     eax, esi
        mov     QWORD  [rbp-40], rdx  ; Guarda el puntero de la cadena a añadir
        mov     BYTE  [rbp-28], al   ; Tipo del nodo
        movzx   eax, BYTE  [rbp-28]
        mov     rdx, QWORD  [rbp-40]
        mov     rsi, rdx
        mov     edi, eax
        call    string_proc_node_create_asm
        mov     QWORD  [rbp-8], rax   ; Nuevo nodo creado
        cmp     QWORD  [rbp-8], 0
        je      .L11               ; Si falla la creación, salta a fin
        mov     rax, QWORD  [rbp-24]
        mov     rax, QWORD  [rax+8]
        test    rax, rax
        jne     .L10               ; Si la lista ya tiene nodos, añade al final
        ; Lista vacía: asigna el nuevo nodo como inicio y fin.
        mov     rax, QWORD  [rbp-24]
        mov     rdx, QWORD  [rbp-8]
        mov     QWORD  [rax], rdx
        mov     rax, QWORD  [rbp-24]
        mov     rdx, QWORD  [rbp-8]
        mov     QWORD  [rax+8], rdx
        jmp     .L7
.L10:
        ; Recorre la lista y añade el nuevo nodo.
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
        ; Concatena las cadenas de la lista según el tipo.
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     QWORD  [rbp-40], rdi  ; Guarda puntero a la lista
        mov     eax, esi
        mov     QWORD  [rbp-56], rdx  ; Guarda el puntero de la cadena base
        mov     BYTE  [rbp-44], al   ; Carácter de comparación
        cmp     QWORD  [rbp-40], 0
        je      .L13              ; Retorna NULL si la lista es NULL
        cmp     QWORD  [rbp-56], 0
        jne     .L14
.L13:
        mov     eax, 0
        jmp     .L15
.L14:
        ; Inicia la cadena concatenada con la base.
        mov     rax, QWORD  [rbp-40]
        mov     rax, QWORD  [rax]
        mov     QWORD  [rbp-8], rax
        mov     rax, QWORD  [rbp-56]
        mov     rdi, rax
        call    strlen
        add     rax, 1
        mov     rdi, rax
        call    malloc
        mov     QWORD  [rbp-16], rax  ; Aloca memoria para la cadena resultante
        cmp     QWORD  [rbp-16], 0
        jne     .L16
        mov     eax, 0
        jmp     .L15
.L16:
        mov     rdx, QWORD  [rbp-56]
        mov     rax, QWORD  [rbp-16]
        mov     rsi, rdx
        mov     rdi, rax
        call    strcpy           ; Copia la cadena base
        jmp     .L17
.L19:
        ; Recorre la lista y concatena si el tipo coincide.
        mov     rax, QWORD  [rbp-8]
        movzx   eax, BYTE  [rax+16]
        cmp     BYTE  [rbp-44], al
        jne     .L18
        mov     rax, QWORD  [rbp-8]
        mov     rdx, QWORD  [rax+24]
        mov     rax, QWORD  [rbp-16]
        mov     rsi, rdx
        mov     rdi, rax
        call    str_concat        ; Concatena la siguiente cadena
        mov     QWORD  [rbp-24], rax
        mov     rax, QWORD  [rbp-16]
        mov     rdi, rax
        call    free             ; Libera la cadena anterior
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