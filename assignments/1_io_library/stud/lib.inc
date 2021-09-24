section .text

string_length:
    xor rax, rax
.loop:
    cmp byte [rdi+rax], 0
    je .end
    inc rax
    jmp .loop
.end:
    ret

print_string:
    push rdi
    call string_length
    pop rsi
    mov rdx, rax
    mov rax, 1
    mov rdi, 1
    syscall
    ret


print_char:
    push rdi
    mov rdi, rsp
    call print_string
    pop rdi
    ret

print_newline:
    xor rax, rax
    ret


print_uint:
    ; backup argument
    mov rax, rdi

    ; reserve buffer in stack
    mov rdi, rsp
    sub rsp, 24

    ; null terminate
    dec rdi
    mov byte [rdi], 0

    ; uint to string
    mov r8, 10
.loop:
    xor rdx, rdx
    div r8
    or  dl, 0x30
    dec rdi 
    mov [rdi], dl
    test rax, rax
    jnz .loop 
   
    call print_string
    
    add rsp, 24
    ret


print_int:
    test rdi, rdi
    jns print_uint

    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    
    neg rdi
    jmp print_uint

string_equals:
    xor rax, rax
    ret


read_char:
    xor rax, rax ; syscall ID
    xor rdi, rdi ; fd of stdin
    push 0
    mov rsi, rsp ; buffer
    mov rdx, 1 ; count
    syscall
    pop rax
    ret

read_word:
    push r14
    push r15

    xor r14, r14 ; r14 = current index
    mov r15, rsi
;    test r15, r15
;    jz .too_long
    dec r15      ; r15 = max index

.before_word:
    push rdi
    call read_char
    pop rdi

    cmp al, ' '
    je .before_word
    cmp al, 0x09
    je .before_word
    cmp al, 0x0A
    je .before_word
    cmp al, 0x0D
    je .before_word

    test al, al
    jz .success

.in_word:
    mov byte [rdi + r14], al
    inc r14

    push rdi
    call read_char
    pop rdi

    cmp al, ' '
    je .success
    cmp al, 0x09
    je .success
    cmp al, 0x0A
    je .success
    cmp al, 0x0D
    je .success
    test al, al
    je .success

    cmp r14, r15
    je .too_long

    jmp .in_word

.success:
    mov byte [rdi + r14], 0
    mov rax, rdi
    mov rdx, r14
    pop r15
    pop r14
    ret

.too_long:
    xor rax, rax
    pop r15
    pop r14
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_uint:
    xor rax, rax
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_int:
    xor rax, rax
    ret 


string_copy:
    push rdi
    push rsi
    push rdx
    call string_length
    pop rdx
    pop rsi
    pop rdi
    cmp rax, rdx
    jae .too_long

    push rsi
.loop:
    mov dl, [rdi]
    mov [rsi], dl
    inc rdi
    inc rsi
    test dl, dl
    jnz .loop    
    pop rax
    ret

.too_long:
    mov rax, 0
    ret
