GLOBAL TesterWrite
GLOBAL TesterRead

TesterWrite:
    mov rbx, string
    mov rdx, 0x0f
    mov rax, 1
    int 80h
    ret

TesterRead:
    mov rbx, strRead
    mov rax, 0
    int 80h
    mov rbx, strRead
    mov rdx, 0x04
    mov rax, 1
    int 80h
    ret

section .data
    string db "Verificado", 0

section .bss
    strRead resb 5

