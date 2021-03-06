
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
;GLOBAL get_registers						; Funcion para obtener el valor de los registros (utilizada en las excepciones)
GLOBAL getCurrContext                       ; Funcion para obtener el valor de los registros (utilizada en el scheduler)

EXTERN irqDispatcher						; Cuando se lance una interrupcion, se llamara a esta funcion para que ejecute la rutina de atencion correspondiente
EXTERN exceptionDispatcher					; Similar a la funcion anterior, pero dedicada a excepciones
EXTERN clear_queue                          ; Limpia el buffer del teclado
EXTERN write_handler                        ; Handler de los syscalls
EXTERN read_handler
EXTERN exec_handler
EXTERN exit_handler
EXTERN time_handler
EXTERN mem_handler
EXTERN tick_handler
EXTERN blink_handler
EXTERN regs_handler
EXTERN clear_handler

SECTION .text

%macro saveRegs 1
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
    push rax
    ;RIP
    mov qword rax, [rsp+8]                  ; Guardo en rax el valor de RIP (direccion de la siguiente instruccion si es una interrupcion, o de donde ocurrio la excepcion)
    mov qword [%1+ 128], rax                 ; Guardo en el arreglo, el valor de RIP
    ;guardo el rsp donde ocurrio la interrupcion/excepcion
    mov qword rax, [rsp + 32]
    mov qword [%1 + 120], rax ;lo guardo para que lo imprima la excepcion
    ;FLAGS: lo guardo para cada proceso
    mov qword rax, [rsp+24] ;guardo los RFLAGS
    mov qword [%1+ 136], rax ;lo guarda en el contexto actual
    pop rax
%endmacro

%macro restoreRegs 1
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
        push rax
        ;RSP
        mov qword rax, [%1 + 120]
        mov qword [rsp+32], rax ;guardo RSP 24+8
        ;RIP
        mov qword rax, [%1+ 128]                     ; Guardo en RAX el valor de RIP obtenido en saveRegs
        mov qword [rsp+8], rax                      ; tengo que hacer rsp+8 por el push de rax
        ;FLAGS
        ;Ahora estoy dejando el valor del que crea el proceso en el caso de que este agregando
        mov qword rax, [%1+ 136] ;guardo los flags antes de la execpcion
        mov qword [rsp+24],rax
        pop rax
%endmacro


; Macro para todas las irq, segun el parametro que paso es como entra en el case de irq
; Es una macro que recibe por argumento el codigo de la interrupcion lanzada y, asi ejecutar la rutina de atencion correspondiente
%macro irqHandlerMaster 1


    saveRegs curr_context
	mov rdi, %1 ; pasaje de parametro (%1 representa el valor del primer, y unico, argumento)
	call irqDispatcher

	; signal pic EOI (End of Interrupt): Se debe avisar al PIC que la interrupción ya fue manejada, enviando el valor 20h al puerto 20h
	mov al, 20h
	out 20h, al


	restoreRegs curr_context
	iretq
%endmacro


; Es una macro que recibe por argumento el codigo de la excepción lanzada y, asi ejecutar la rutina de atencion correspondiente
%macro exceptionHandler 1

    saveRegs curr_context
	mov rdi, %1                         ; Pasaje de parametro
	call exceptionDispatcher            ; Llamamos al dispatcher de excepciones
    restoreRegs curr_context

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
    saveRegs curr_context                       ; Resguardamos los registros
    cmp rax, 0
    jz sys_read                                 ; Syscall para read (id = 0)
    cmp rax, 1
    jz sys_write                                ; Syscall para write (id = 1)
    cmp rax, 2
    jz sys_exec                                 ; Syscall para exec (id = 2)
    cmp rax, 3
    jz sys_exit                                 ; Syscall para exit (id = 3)
    cmp rax, 4
    jz sys_time                                 ; Syscall para time (id = 4)
    cmp rax, 5
    jz sys_mem                                  ; Syscall para mem (id = 5)
    cmp rax, 6
    jz sys_tick                                 ; Syscall para timer ticks (id = 6)
    cmp rax, 7
    jz sys_blink                                ; Syscall para video blink (id = 7)
    cmp rax, 8
    jz sys_regs                                 ; Syscall para devolver un screenshot de los registros (id = 8)
    cmp rax, 9
    jz sys_clear                                ; Syscall para limpiar el driver de video
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
;   rbx: cantidad de programas que se desea correr (1 o 2)
;   rcx: vector con 1 o 2 program_t para inicializar a los programas (si son 2, el primero es el de la izquierda)
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: 0 si logro correr el programa; -1 si no
;------------------------------------------------------------------------------------
sys_exec:
    mov rdi, rbx ;paso la cantidad de programas
    mov rsi, rcx ;paso el vector de program_t
    call exec_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Exit: Termina la ejecucion del proceso que la llama
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: 0 si pudo terminarlo, -1 si no
;------------------------------------------------------------------------------------
sys_exit:
    call exit_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Time: Devuelve el valor para la unidad de tiempo indicada en el parametro
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: 0 = secs, 2 = mins, 4 = hour, 6 = day of week, 7 = day, 8 = month, 9 = year
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: el valor para el tipo de unidad indicada
;------------------------------------------------------------------------------------
sys_time:
    mov rdi, rbx
    call time_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Mem: Devuelve un arreglo con la informacion de los siguientes 32 bytes de memoria
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: Direccion de memoria inicial sobre el cual consultar
;   rcx: La direccion de memoria donde comienza el arreglo
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: Cantidad de elementos en el arreglo (max = 32)
;------------------------------------------------------------------------------------
sys_mem:
    mov rdi, rbx
    mov rsi, rcx
    call mem_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Tick: Devuelve la cantidad de ticks realizados por el timer tick desde que inicio la computadora
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: Cantidad de ticks
;------------------------------------------------------------------------------------
sys_tick:
    call tick_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Blink: Indica donde se escribira el próximo caracter en pantalla
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: 0 
;------------------------------------------------------------------------------------
sys_blink:
    call blink_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Regs: Devuelve el estado de los registros de la ultima vez que se realizo CTRL+S
;-------------------------------------------------------------------------------------
; Parametros:
;   rbx: Un arreglo con 18 espacios para enteros de 64 bits
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: 0 si todos los registros estan en 0, 1 si ya se realizo CTRL+S
;------------------------------------------------------------------------------------
sys_regs:
    mov rdi, rbx
    call regs_handler
    jmp fin

;-------------------------------------------------------------------------------------
; sys_clear: Limpiar la terminal de comandos
;-------------------------------------------------------------------------------------
; Parametros:
;   void
;-------------------------------------------------------------------------------------
; Retorno:
;------------------------------------------------------------------------------------
sys_clear:
    call clear_handler
    jmp fin

fin:
    mov [aux], rax                                  ; Resguardamos el valor de retorno (ojo que aux es una direccion, y queremos guardar adentro)
    restoreRegs curr_context                        ; Recuperamos los registros
    mov rax, [aux]                                  ; Recuperamos el valor de retorno en rax
    iretq




;-------------------------------------------------------------------------------------
; getCurrContext: obtener los valores de los registros
;-------------------------------------------------------------------------------------
; parametros:
;   void
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

;get_registers:
;	push rbp
;	mov rbp, rsp
;
;	mov rax, curr_context                                ; Devolvemos el arreglo con los registros en el momento de la excepcion
;
;	mov rsp, rbp
;	pop rbp
;	ret

haltcpu:
	cli
	hlt
	ret



SECTION .bss
;----------------------------------------------------------------------
;   curr_context: vector que antes de los handlers tiene el contexto del proceso en el que ocurrio la interrupcio
;   o excepcion, y luego de los handlers debe tener el contexto del proceso que se quiere ejecutar
;----------------------------------------------------------------------
	curr_context resb 144       ; Contexto del programa (para almacenar procesos)
	aux resq 1                  ; Variable auxiliar para resguardar el valor de retorno de las syscalls

