
GLOBAL _cli									; Ejecuta la instruccion cli, la cual inhabilita las interrupciones
GLOBAL _sti									; Ejecuta la instruccion sti, la cual vuelve a habilitar las instrucciones
GLOBAL picMasterMask						; Permite modificar las interrupciones que se van a poder habilitar (0 = on, 1 = off), recibiendo por parametros un numero de 16 bits (aunque solo se utilicen los primeros 8), del pic maestro
GLOBAL picSlaveMask							; Permite modificar las interrupciones que se van a poder habilitar (0 = on, 1 = off), recibiendo por parametros un numero de 16 bits (aunque solo se utilicen los primeros 8), del pic esclavo
GLOBAL haltcpu								; Esta funcion es más restrictiva que la siguiente, pues detiene el procesador (hlt) pero solo puede reejcutar con una interrupcion no enmascarable (es decir, aquellas que no pueden ser detenidas con cli)
GLOBAL _hlt									; Es una funcion que contiene la instruccion HLT (halt) la cual provoca que el procesador deje de ejecutar instrucciones hasta la proxima interrupcion externa (https://www.eeeguide.com/8086-external-hardware-synchronization-instructions/)

GLOBAL _irq00Handler						; Las siguientes son funciones que ejecutan la interrupcion cuyo codigo IRQ está indicado en el nombre
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _syscallHandler                      ; Ejecuta las interrupciones de software

GLOBAL _exception0Handler					; Es una funcion que ejecuta la excepcion de id 0 (division por cero)
GLOBAL _exception6Handler					; Es una funcion que ejecuta la excepcion de id 6 (invalid opcode)
GLOBAL getRegisters							; Funcion para obtener el valor de los registros
GLOBAL getCurrContext

EXTERN irqDispatcher						; Cuando se lance una interrupcion, se llamara a esta funcion para que ejecute la rutina de atencion correspondiente
EXTERN exceptionDispatcher					; Similar a la funcion anterior, pero dedicada a excepciones

EXTERN write_handler
EXTERN read_handler
EXTERN exec_handler
EXTERN exit_handler

SECTION .text

; Es una macro que guarda en un arreglo pasado por parametros todos los registros
%macro saveRegs 1
    pushfq                              ; Pushea los flags
    pop qword [%1 + 136]               ; Los guarda en la estructura auxiliar
    mov [%1], r8
	mov [%1+8], r9
	mov [%1+16], r10
	mov [%1+24], r11
	mov [%1+32], r12
	mov [%1+40], r13
	mov [%1+48], r14
	mov [%1+56], r15
	mov [%1+64], rax
	mov [%1+72], rbx
	mov [%1+80], rcx
	mov [%1+88], rdx
	mov [%1+96], rsi
	mov [%1+104], rdi
	mov [%1+112], rbp
    mov [%1+120], rsp
    push rax
    mov qword rax, [rsp+8]                  ; Guardo en rax el valor de RIP (La siguiente instruccion luego de lanzar la excepcion)
    mov qword [%1+128], rax                 ; Guardo en el arreglo, el valor de RIP
    pop rax
%endmacro

; Es una macro que recupera el valor de los registros desde un arreglo pasado por parametros
%macro restoreRegs 1
        push qword[%1 + 136]                ; Pushea los flags (solo se pueden restaurar los flags desde el stack)
        popfq                               ; Restarura los flags
        mov r8, qword[%1]
    	mov r9, qword[%1+8]
    	mov r10, qword[%1+16]
    	mov r11, qword[%1+24]
    	mov r12, qword[%1+32]
    	mov r13, qword[%1+40]
    	mov r14, qword[%1+48]
    	mov r15, qword[%1+56]
    	mov rax, qword[%1+64]
    	mov rbx, qword[%1+72]
    	mov rcx, qword[%1+80]
    	mov rdx, qword[%1+88]
    	mov rsi, qword[%1+96]
    	mov rdi, qword[%1+104]
    	mov rbp, qword[%1+112]
        mov rsp, qword[%1+120]                      ; Esto es lo que hace el cambio de contexto, se mueve a otro punto del stack
        push rax
        mov qword rax, [%1+128]                     ; Guardo en RAX el valor de RIP obtenido en saveRegs
        mov qword [rsp+8], rax                      ; tengo que hacer rsp+8 por el push de rax
        ;nota: gdb agrega en el stach un elemento que es la funcion donde esta el ususario en este momento
        ;ese no es un elemento del stack, es solo para que el usuario sepa donde esta
        pop rax
%endmacro
; Es una macro que recibe 0 argumentos y pushea al stack todos los registros para resguardarlos
%macro pushState 0							
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

; Es una macro que recibe 0 argumentos y popea del stack todos los registros para recuperarlos
%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

; Macro para todas las irq, segun el parametro que paso es como entra en el case de irq
; Es una macro que recibe por argumento el codigo de la interrupcion lanzada y, asi ejecutar la rutina de atencion correspondiente
%macro irqHandlerMaster 1
	;pushState
    saveRegs curr_context
	mov rdi, %1 ; pasaje de parametro (%1 representa el valor del primer, y unico, argumento)
	call irqDispatcher

	; signal pic EOI (End of Interrupt): Se debe avisar al PIC que la interrupción ya fue manejada, enviando el valor 20h al puerto 20h
	mov al, 20h
	out 20h, al

	;popState
	restoreRegs curr_context
	iretq
%endmacro


; Es una macro que recibe por argumento el codigo de la excepción lanzada y, asi ejecutar la rutina de atencion correspondiente
%macro exceptionHandler 1
    saveRegs exc_state
    ;pushState                   ; Guarda en un arreglo el valor de los registros al lanzar una excepcion
	mov rdi, %1                         ; Pasaje de parametro
	call exceptionDispatcher            ; Llamamos al dispatcher de excepciones
    ;popState
    restoreRegs exc_state
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al						; Se enmascara las interrupciones del PIC maestro por el puerto 21h
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al						; Se enmascara las interrupciones del PIC esclavo por el puerto 21h
    pop     rbp
    retn


;--------------------------------------------------------------------------------------
; Interrupciones
;--------------------------------------------------------------------------------------

; poner buenos nombres mas descriptivos
;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;--------------------------------------------------------------------------------------
; Excepciones
;--------------------------------------------------------------------------------------

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid opcode Exception
_exception6Handler:
	exceptionHandler 6

;-------------------------------------------------------------------------------------
; _syscallHandler: Handler para las interrupciones (int 80h)
;-------------------------------------------------------------------------------------
; Parametros:
;   rax: el Id de la inter
;   rbx --> rxx : parametros variables en funcion de la interupcion.
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: Depende de la syscall, o -2 si no es un parametro valido
;------------------------------------------------------------------------------------
_syscallHandler:
    pushState                                   ; Resguardamos los registros
    cmp rax, 0
    jz sys_read                                 ; Syscall para read (id = 0)
    cmp rax, 1
    jz sys_write                                ; Syscall para write (id = 1)
    cmp rax, 2
    jz sys_exec                                 ; Syscall para exec (id = 2)
    cmp rax, 3
    jz sys_exit                                 ; Syscall para exit (id = 4)
    mov rax, -2                                 ; Si no es una funcion conocida, se devuelve -2 en rax
    jmp fin

; ------------------------------------------
; Read: Lee el siguiente caracter ingresado por pantalla
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: el string donde copiar el caracter (tamaño minimo: 2)
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: cantidad de caracteres leidos
;-------------------------------------------------------------------------------------
sys_read:
    mov rdi, rbx
    call read_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Write: Escribe por pantalla
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: string a imprimir
;   rcx: formato de impresion (un color definido previamente)
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: cantidad de caracteres escritos
;------------------------------------------------------------------------------------
sys_write:
    mov rdi, rbx
    mov rsi, rcx
    call write_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Exec: Guarda un proceso
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: puntero a la primera instruccion del programa
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: 0 si logro correr el programa; -1 si no
;------------------------------------------------------------------------------------
sys_exec:
    mov rdi, rbx
    call exec_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Exit: Termina la ejecucion
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: codigo de error
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: numero de proceso
;------------------------------------------------------------------------------------
sys_exit:
    mov rdi, rbx
    call exit_handler
    jmp fin

fin:
    mov [aux], rax                                  ; Resguardamos el valor de retorno (ojo que aux es una direccion, y queremos guardar adentro)
    popState                                        ; Recuperamos los registros
    mov rax, [aux]                                  ; Recuperamos el valor de retorno en rax
    iret




;-------------------------------------------------------------------------------------
; getCurrContext: obtener los valores de los registros
;-------------------------------------------------------------------------------------
; parametros:
;   null
;-----------------------------------;----------------------------------------------------------------------------------------------------------------------------------
; retorno:
; 	vector con el valor de los registros ordenados segun:
;	"R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "
;-------------------------------------------------------------------------------------
getCurrContext:
    push rbp
    mov rbp, rsp

    mov rax, curr_context

    mov rsp, rbp
    pop rbp
    ret

getRegisters:
	push rbp
	mov rbp, rsp

;	mov [reg], r8
;	mov [reg+8], r9
;	mov [reg+16], r10
;	mov [reg+24], r11
;	mov [reg+32], r12
;	mov [reg+40], r13
;	mov [reg+48], r14
;;	mov [reg+56], r15
;	mov [reg+64], rax
;	mov [reg+72], rbx
;	mov [reg+80], rcx
;	mov [reg+88], rdx
;	mov [reg+96], rsi
;	mov [reg+104], rdi
	;claramente como esta no es, definir una manera
	;opcion 1: fijarse en que lugar del stack estan los rbp, rsp y rip y restarle eso al rsp actual.
	;opcion 2: No hacer nada de esto e imprimir los registros en el momento de la interupcion (antes del int 0h)
	;desventaja de 2 es que es medio chanta porque la interupcion no haria nada
	;opcion 3: ir pasando como parametro el rbp, rsp y rip entre las funciones e imprimirlo al final.
	;opcion 4: usar TSS pero no es la utilidad para esto, es mas general
;	mov [reg+112], rbp
;	mov [reg+120], rsp
;	mov [reg+128], rip

	mov rax, exc_state                                ; Devolvemos el arreglo con los registros en el momento de la excepcion

	mov rsp, rbp
	pop rbp
	ret

haltcpu:
	cli
	hlt
	ret


; TODO: Usar un unico arreglo para los registros
SECTION .bss
	exc_state resb 144		    ; Guarda 8*18 lugares de memoria (para los 18 registros)
	curr_context resb 144       ; Contexto del programa (para almacenar procesos)
	aux resq 1                  ; Variable auxiliar