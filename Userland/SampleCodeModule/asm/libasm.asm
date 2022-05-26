GLOBAL sys_write
GLOBAL sys_read
GLOBAL zero_division_exc
GLOBAL invalid_opcode_exc
GLOBAL get_week
GLOBAL get_day
GLOBAL get_month
GLOBAL get_year
GLOBAL get_hour
GLOBAL get_min
GLOBAL sys_exec
GLOBAL sys_exit
GLOBAL get_registers
GLOBAL get_memory


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

    ;mov rbx, rdi
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

;    mov rcx, 0
;    mov rax, 1
;	 div rcx                 ; Hacemos la division 1 / 0

    int 0h

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

;-------------------------------------------------------------------------------------
; FUNCIONES PARA MANEJO DE FECHA Y HORA
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: categoria horaria que quiero segun : typedef enum {SEC = 0, MIN = 2, HOUR = 4, DAY_WEEK = 6, DAY_MONTH = 7, MONTH = 8, YEAR = 9} timeType;
;   rax: la sys_call
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: entero con la categoria que quiero
;-------------------------------------------------------------------------------------
get_week:
    push rbp
    mov rbp, rsp

    mov rbx, 6
    mov rax, 4
    int 80h

	mov rsp, rbp
	pop rbp
	ret

get_day:
    push rbp
    mov rbp, rsp

    mov rbx, 7
    mov rax, 4
    int 80h

	mov rsp, rbp
	pop rbp
	ret

get_month:
    push rbp
    mov rbp, rsp

    mov rbx, 8
    mov rax, 4
    int 80h

	mov rsp, rbp
	pop rbp
	ret

get_year:
    push rbp
    mov rbp, rsp

    mov rbx, 9
    mov rax, 4
    int 80h

	mov rsp, rbp
	pop rbp
	ret

get_hour:
    push rbp
    mov rbp, rsp

    mov rbx, 4
    mov rax, 4
    int 80h

	mov rsp, rbp
	pop rbp
	ret

get_min:
    push rbp
    mov rbp, rsp

    mov rbx, 2
    mov rax, 4
    int 80h

	mov rsp, rbp
	pop rbp
	ret
;-------------------------------------------------------------------------------------
; FIN FUNCIONES PARA MANEJO DE FECHA Y HORA
;-------------------------------------------------------------------------------------


;-------------------------------------------------------------------------------------
; get_registers: Obtener el valor de los registros
;-------------------------------------------------------------------------------------
; Parametros:
;-------------------------------------------------------------------------------------
; Retorno:
;   el vector con el valor de los registros
;------------------------------------------------------------------------------------
get_registers:
    push rbp
    mov rbp, rsp

    pushfq                              ; Pushea los flags
    pop qword [reg + 136]                ; Los guarda en la estructura auxiliar
    mov [reg], r8
	mov [reg+8], r9
	mov [reg+16], r10
	mov [reg+24], r11
	mov [reg+32], r12
	mov [reg+40], r13
	mov [reg+48], r14
	mov [reg+56], r15
	mov [reg+64], rax
	mov [reg+72], rbx
	mov [reg+80], rcx
	mov [reg+88], rdx
	mov [reg+96], rsi
	mov [reg+104], rdi
	pop rax
	mov [reg+112], rax
	push rax
    mov [reg+120], rbp                      ; deberia ir rsp pero por stack frame dejo este
    mov qword rax, [rbp+8]                  ; Guardo en rax el valor de RIP
    mov qword [reg+128], rax                ; Guardo en el arreglo, el valor de RIP

    mov rax, reg

    mov rbp, rsp
    pop rbp
    ret


;-------------------------------------------------------------------------------------
; get_memory: Obtener un vuelco de memoria de 32bits
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: puntero al lugar de memoria donde realizar el vuelco
;-------------------------------------------------------------------------------------
; Retorno:
;   el valor de la direccion de memoria de 32bits
;------------------------------------------------------------------------------------
get_memory:
    push rbp
    mov rbp, rsp

    mov rax, 0
    mov eax, [rdi]

    mov rbp, rsp
    pop rbp
    ret

SECTION .bss
	reg resb 144		    ; Guarda 8*18 lugares de memoria (para los 18 registros)
