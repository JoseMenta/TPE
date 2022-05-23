GLOBAL TesterWrite
GLOBAL TesterRead
GLOBAL zero_division_exc

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

zero_division_exc:
	push rbp
	mov rbp, rsp

	int 0h

	mov rsp, rbp
	pop rbp
	ret

section .data
    string db "Verificado", 0

section .bss
    strRead resb 5

