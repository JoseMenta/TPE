GLOBAL sys_write
GLOBAL sys_read
GLOBAL zero_division_exc
GLOBAL invalid_opcode_exc

section .text

;-------------------------------------------------------------------------------------
; sys_write: Llama a la sys_call de escritura
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: el string a imprimir
;   rsi: el formato con el que se imprime el texto
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: cantidad de caracteres escritos
;------------------------------------------------------------------------------------
sys_write:
    push rbp
    mov rbp, rsp            ; Armado de stack frame

    mov rbx, rdi            ; La sys_call recibe por rbx el string a imprimir
    mov rcx, rsi            ; La sys_call recibe por rcx el formato
    mov rax, 1
    int 80h

    mov rsp, rbp            ; Desarmado de stack frame
    pop rbp
    ret


;-------------------------------------------------------------------------------------
; sys_read: Llama a la sys_call de lectura
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: El puntero al string sobre el cual copiar el caracter leido del teclado
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: cantidad de caracteres leidos
;------------------------------------------------------------------------------------
sys_read:
    push rbp
    mov rbp, rsp

    mov rbx, rdi            ; La sys_call recibe por rbx la posicion de memoria donde se guarda el caracter
    mov rax, 0
    int 80h

    mov rsp, rbp
    pop rbp
    ret


;-------------------------------------------------------------------------------------
; sys_exec: Ejecuta uno o dos nuevos procesos
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: la cantidad de nuevos procesos a ejecutar
;   rsi: vector con el/los nuevo/s proceso/s a ejecutar
;-------------------------------------------------------------------------------------
; Retorno: 0 si resulto exitoso, -1 si no (se agregaron mas de dos procesos)
;------------------------------------------------------------------------------------
sys_exec:
    push rbp
    mov rbp, rsp

    mov rbx, rdi
    mov rcx, rsi
    mov rax, 2
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;-------------------------------------------------------------------------------------
; sys_exit: Finaliza el proceso ejecutandose en el momento
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: el codigo de error
;-------------------------------------------------------------------------------------
; Retorno:
;   el codigo de error
;------------------------------------------------------------------------------------
sys_exit:
    push rbp
    mov rbp, rsp

    mov rbx, rdi
    mov rax, 3
    int 80h

    mov rsp, rbp
    pop rbp
    ret

;-------------------------------------------------------------------------------------
; zero_division_exc: Programa para generar un excepcion de division por cero
;-------------------------------------------------------------------------------------
; Parametros:
;   void zero_division_exc();
;-------------------------------------------------------------------------------------
zero_division_exc:
	push rbp
	mov rbp, rsp

    mov rcx, 0
    mov rax, 1
	div rcx                 ; Hacemos la division 1 / 0

	mov rsp, rbp
	pop rbp
	ret

;-------------------------------------------------------------------------------------
; invalid_opcode_exc: Programa para generar un excepcion de operador invalido
;-------------------------------------------------------------------------------------
; Parametros:
;   null
;-------------------------------------------------------------------------------------
invalid_opcode_exc:
	push rbp
	mov rbp, rsp

	ud2                     ; Genera un invalid opcode

	mov rsp, rbp
	pop rbp
	ret
