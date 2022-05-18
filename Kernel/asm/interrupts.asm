
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


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

haltcpu:
	cli
	hlt
	ret



SECTION .bss
	aux resq 1