
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

GLOBAL _exception0Handler					; Es una funcion que ejecuta la excepcion de id 0 (division por cero)
GLOBAL _exception6Handler					; Es una funcion que ejecuta la excepcion de id 6 (invalid opcode)
GLOBAL getRegisters							; Funcion para obtener el valor de los registros

EXTERN irqDispatcher						; Cuando se lance una interrupcion, se llamara a esta funcion para que ejecute la rutina de atencion correspondiente
EXTERN exceptionDispatcher					; Similar a la funcion anterior, pero dedicada a excepciones

SECTION .text

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
;macro para todas las irq, segun el parametro que paso es como entra en el case de irq
; Es una macro que recibe por argumento el codigo de la interrupcion lanzada y, asi ejecutar la rutina de atencion correspondiente
%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro (%1 representa el valor del primer, y unico, argumento)
	call irqDispatcher

	; signal pic EOI (End of Interrupt): Se debe avisar al PIC que la interrupción ya fue manejada, enviando el valor 20h al puerto 20h
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro


; Es una macro que recibe por argumento el codigo de la excepción lanzada y, asi ejecutar la rutina de atencion correspondiente
%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	popState
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
; getRegisters: obtener los valores de los registros
;-------------------------------------------------------------------------------------
; parametros: - 
;-------------------------------------------------------------------------------------
; retorno:
; 	vector con el valor de los registros
;	ordenados segun el orden de los registros:
;	"R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "
;-------------------------------------------------------------------------------------

getRegisters:
	push rbp
	mov rbp, rsp

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
	;claramente como esta no es, definir una manera
	;opcion 1: fijarse en que lugar del stack estan los rbp, rsp y rip y restarle eso al rsp actual.
	;opcion 2: No hacer nada de esto e imprimir los registros en el momento de la interupcion (antes del int 0h)
	;desventaja de 2 es que es medio chanta porque la interupcion no haria nada
	;opcion 3: ir pasando como parametro el rbp, rsp y rip entre las funciones e imprimirlo al final.
	;mov [reg+112], rbp
	;mov [reg+120], rsp
	;mov [reg+128], rip

	mov rax, reg

	mov rsp, rbp
	pop rbp
	ret


haltcpu:
	cli
	hlt
	ret



SECTION .bss
	reg resq 18		; guarda 8*18 lugares de memoria
	aux resq 1