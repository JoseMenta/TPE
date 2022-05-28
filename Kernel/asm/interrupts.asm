
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
GLOBAL get_registers							; Funcion para obtener el valor de los registros
GLOBAL getCurrContext

EXTERN irqDispatcher						; Cuando se lance una interrupcion, se llamara a esta funcion para que ejecute la rutina de atencion correspondiente
EXTERN exceptionDispatcher					; Similar a la funcion anterior, pero dedicada a excepciones

EXTERN write_handler                        ; Handler de los syscalls
EXTERN read_handler
EXTERN exec_handler
EXTERN exit_handler
EXTERN time_handler
EXTERN mem_handler

SECTION .text
;TODO: guardar los 2 rsp, y poner la diferencia en base a eso
;; Es una macro que guarda en un arreglo pasado por parametros todos los registros
;%macro saveRegs 1
;    pushfq                              ; Pushea los flags
;    pop qword [%1 + 136]               ; Los guarda en la estructura auxiliar
;    mov [%1], r8
;	mov [%1+8], r9
;	mov [%1+16], r10
;	mov [%1+24], r11
;	mov [%1+32], r12
;	mov [%1+40], r13
;	mov [%1+48], r14
;	mov [%1+56], r15
;	mov [%1+64], rax
;	mov [%1+72], rbx
;	mov [%1+80], rcx
;	mov [%1+88], rdx
;	mov [%1+96], rsi
;	mov [%1+104], rdi
;	mov [%1+112], rbp
;    mov [%1+120], rsp  ;esto guarda el rsp en el handler que esta un poco mas arriba de donde se interrumpio
;    ;En la estructura de curr_contex guardo este, despues vemos si hay que cambiar la macro para las excepciones
;    ;asi guardamos el rsp de donde ocurre la excepcion
;    push rax
;    push rbx
;    mov rax, rsp
;    mov rbx, qword[rsp + 40] ;el rsp de antes 24+8+8
;    sub rbx, rax ;la diferencia entre los rsp es el ancho del stack frame, la guardo para armar el stack del otro
;    mov qword[off],rbx
;    pop rbx
;    ;RIP
;    mov qword rax, [rsp+8]                  ; Guardo en rax el valor de RIP (La siguiente instruccion luego de lanzar la excepcion)
;    mov qword [%1+128], rax                 ; Guardo en el arreglo, el valor de RIP
;
;    ;CS
;    mov qword rax, [rsp+16] ;guardo el CS en el stack
;    mov qword [iretq_registers], rax
;
;    ;FLAGS
;    mov qword rax, [rsp+24] ;guardo los RFLAGS
;    mov qword [iretq_registers+8], rax
;
;    ;RSP
;    ;mov qword rax, [rsp + 32] ;sumo 8 a lo que puse en notas porque esta rax
;    ;mov qword[iretq_registers+16], rax ;guardo el rsp del que la "llama"
;    ;RSP guardo el del momento donde llega al handler
;    ;Despues le resto 40 cuando cambio el contexto
;
;    ;SS
;    mov qword rax, [rsp + 40] ;guardo SS
;    mov qword [iretq_registers + 24], rax
;
;    ;Otra cosa que falta y esta ahi (creo que es el alineamiento de stack)
;;    mov qword rax, [rsp + 48]
;;    mov qword [iretq_registers + 32], rax
;
;
;    pop rax
;%endmacro
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
    mov [%1+ 120], rsp  ;esto guarda el rsp en el handler que esta un poco mas arriba de donde se interrumpio
    ;En la estructura de curr_contex guardo este, despues vemos si hay que cambiar la macro para las excepciones
    ;asi guardamos el rsp de donde ocurre la excepcion
    push rax
    push rbx
    ;off va a considerar la diferencia con rsp aumentado 2 lugares
    ;por rax y rbx
    ;Pareceria que se deberia sacar, pero cuando se restaura
    ;Tambien se pushea a rax y rbx antes, entonces la diferencia los
    ;debe considerar
    mov rax, rsp
    mov rbx, qword[rsp + 40] ;el rsp de antes 24+8+8
    sub rbx, rax ;la diferencia entre los rsp es el ancho del stack frame, la guardo para armar el stack del otro
    mov qword[off],rbx
    pop rbx
    ;RIP
    mov qword rax, [rsp+8]                  ; Guardo en rax el valor de RIP (La siguiente instruccion luego de lanzar la excepcion)
    mov qword [%1+ 128], rax                 ; Guardo en el arreglo, el valor de RIP

    ;CS: deberia ser igual para todos
    mov qword rax, [rsp+16] ;guardo el CS en el stack
    mov qword [iretq_registers], rax

    ;FLAGS: lo guardo para cada proceso
    mov qword rax, [rsp+24] ;guardo los RFLAGS
    mov qword [%1+ 144], rax ;lo guarda en el contexto actual
    ;mov qword [iretq_registers + 8],rax
    ;RSP
    ;mov qword rax, [rsp + 32] ;sumo 8 a lo que puse en notas porque esta rax
    ;mov qword[iretq_registers+16], rax ;guardo el rsp del que la "llama"
    ;RSP guardo el del momento donde llega al handler
    ;Despues le resto 40 cuando cambio el contexto

    ;SS: deberia ser igual para todos
    mov qword rax, [rsp + 40] ;guardo SS
    mov qword [iretq_registers + 24], rax

    ;Otra cosa que falta y esta ahi (creo que es el alineamiento de stack)
;    mov qword rax, [rsp + 48]
;    mov qword [iretq_registers + 32], rax


    pop rax
%endmacro
;%macro saveRegs 1
;    pushfq                              ; Pushea los flags (en el handler de la excepcion)
;    pop qword [%1 + rflags_o]               ; Los guarda en la estructura auxiliar
;    mov [%1+ r8_o], r8
;	mov [%1+ r9_o], r9
;	mov [%1+ r10_o], r10
;	mov [%1+ r11_o], r11
;	mov [%1+ r12_o], r12
;	mov [%1+ r13_o], r13
;	mov [%1+ r14_o], r14
;	mov [%1+ r15_o], r15
;	mov [%1+ rax_o], rax
;	mov [%1+ rbx_o], rbx
;	mov [%1+ rcx_o], rcx
;	mov [%1+ rdx_o], rdx
;	mov [%1+ rsi_o], rsi
;	mov [%1+ rdi_o], rdi
;	mov [%1+ rbp_o], rbp
;    mov [%1+ rsp_o], rsp  ;esto guarda el rsp en el handler que esta un poco mas arriba de donde se interrumpio
;    ;En la estructura de curr_contex guardo este, despues vemos si hay que cambiar la macro para las excepciones
;    ;asi guardamos el rsp de donde ocurre la excepcion
;    push rax
;    push rbx
;    mov rax, rsp
;    mov rbx, qword[rsp + 40] ;el rsp de antes 24+8+8
;    sub rbx, rax ;la diferencia entre los rsp es el ancho del stack frame, la guardo para armar el stack del otro
;    mov qword[off],rbx
;    pop rbx
;    ;RIP
;    mov qword rax, [rsp+8]                  ; Guardo en rax el valor de RIP (La siguiente instruccion luego de lanzar la excepcion)
;    mov qword [%1+ rip_o], rax                 ; Guardo en el arreglo, el valor de RIP
;
;    ;CS: deberia ser igual para todos
;    mov qword rax, [rsp+16] ;guardo el CS en el stack
;    mov qword [iretq_registers], rax
;
;    ;FLAGS: lo guardo para cada proceso
;    mov qword rax, [rsp+24] ;guardo los RFLAGS
;    mov qword [%1+ a_rflags_o], rax ;lo guarda en el contexto actual
;    ;mov qword [iretq_registers + 8],rax
;    ;RSP
;    ;mov qword rax, [rsp + 32] ;sumo 8 a lo que puse en notas porque esta rax
;    ;mov qword[iretq_registers+16], rax ;guardo el rsp del que la "llama"
;    ;RSP guardo el del momento donde llega al handler
;    ;Despues le resto 40 cuando cambio el contexto
;
;    ;SS: deberia ser igual para todos
;    mov qword rax, [rsp + 40] ;guardo SS
;    mov qword [iretq_registers + 24], rax
;
;    ;Otra cosa que falta y esta ahi (creo que es el alineamiento de stack)
;;    mov qword rax, [rsp + 48]
;;    mov qword [iretq_registers + 32], rax
;
;
;    pop rax
;%endmacro
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

        push rbx
        mov rbx, qword[off] ;guardo el offset de antes
        mov rax, rsp ;el rsp nuevo
        add rax, rbx ;le dejo la misma diferencia que en el stack que lo llama
        ;lea rax, [rsp + 56] ;esta 48 + 8 (por rax) = 56 mas abajo
        ;mov qword rax, [iretq_registers+16]
        mov qword [rsp+40], rax ;guardo RSP 24+8+8
        pop rbx




        ;nota: gdb agrega en el stack un elemento que es la funcion donde esta el ususario en este momento
        ;ese no es un elemento del stack, es solo para que el usuario sepa donde esta
        ;Esto lo agrego por lo que vi de iretq
        ;RIP
        mov qword rax, [%1+ 128]                     ; Guardo en RAX el valor de RIP obtenido en saveRegs
        mov qword [rsp+8], rax                      ; tengo que hacer rsp+8 por el push de rax

        ;CS
        mov qword rax, [iretq_registers]
        mov qword [rsp+16],rax

        ;FLAGS TODO: ver si este tiene que ser un valor especial para poder volver (Alejo dice que hay un flag que tiene que tener si esta en una interrupcion)
        ;Ahora estoy dejando el valor del que crea el proceso en el caso de que este agregando
        mov qword rax, [%1+ 144] ;guardo los flags antes de la execpcion
        ;mov qword rax, [iretq_registers + 8]
        mov qword [rsp+24],rax


        ;mov rax, rsp
        ;sub rax, 48h ;va a ser 56 mas abajo del que estaba en el handler (para considerar a los cambios por el stack frame de la interrupcion)


        ;SS
        mov qword rax, [iretq_registers +32] ;restauro SS
        mov qword [rsp+40], rax

        ;Alineamiento de memoria
;        mov qword rax, [iretq_registers + 32]
;        mov qword [rsp + 48], rax

        pop rax
%endmacro
; Es una macro que recupera el valor de los registros desde un arreglo pasado por parametros
;%macro restoreRegs 1
;        push qword[%1 + rflags_o]                ; Pushea los flags (solo se pueden restaurar los flags desde el stack)
;        popfq                               ; Restarura los flags
;        mov r8, qword[%1 + r8_o]
;    	mov r9, qword[%1+ r9_o]
;    	mov r10, qword[%1+ r10_o]
;    	mov r11, qword[%1+ r11_o]
;    	mov r12, qword[%1+ r12_o]
;    	mov r13, qword[%1+ r13_o]
;    	mov r14, qword[%1+ r14_o]
;    	mov r15, qword[%1+ r15_o]
;    	mov rax, qword[%1+ rax_o]
;    	mov rbx, qword[%1+72+ rbx_o]
;    	mov rcx, qword[%1+80 + rcx_o]
;    	mov rdx, qword[%1+88 + rdx_o]
;    	mov rsi, qword[%1+96 + rsi_o]
;    	mov rdi, qword[%1+104 + rdi_o]
;    	mov rbp, qword[%1+112 + rbp_o]
;        mov rsp, qword[%1+120 + rsp_o]                      ; Esto es lo que hace el cambio de contexto, se mueve a otro punto del stack
;
;        push rax
;
;        push rbx
;        mov rbx, qword[off] ;guardo el offset de antes
;        mov rax, rsp ;el rsp nuevo
;        add rax, rbx ;le dejo la misma diferencia que en el stack que lo llama
;        ;lea rax, [rsp + 56] ;esta 48 + 8 (por rax) = 56 mas abajo
;        ;mov qword rax, [iretq_registers+16]
;        mov qword [rsp+40], rax ;guardo RSP 24+8+8
;        pop rbx
;
;
;
;
;        ;nota: gdb agrega en el stack un elemento que es la funcion donde esta el ususario en este momento
;        ;ese no es un elemento del stack, es solo para que el usuario sepa donde esta
;        ;Esto lo agrego por lo que vi de iretq
;        ;RIP
;        mov qword rax, [%1+ rip_o]                     ; Guardo en RAX el valor de RIP obtenido en saveRegs
;        mov qword [rsp+8], rax                      ; tengo que hacer rsp+8 por el push de rax
;
;        ;CS
;        mov qword rax, [iretq_registers]
;        mov qword [rsp+16],rax
;
;        ;FLAGS TODO: ver si este tiene que ser un valor especial para poder volver (Alejo dice que hay un flag que tiene que tener si esta en una interrupcion)
;        ;Ahora estoy dejando el valor del que crea el proceso en el caso de que este agregando
;        mov qword rax, [%1+ a_rflags_o] ;guardo los flags antes de la execpcion
;        ;mov qword rax, [iretq_registers + 8]
;        mov qword [rsp+24],rax
;
;
;        ;mov rax, rsp
;        ;sub rax, 48h ;va a ser 56 mas abajo del que estaba en el handler (para considerar a los cambios por el stack frame de la interrupcion)
;
;
;        ;SS
;        mov qword rax, [iretq_registers +32] ;restauro SS
;        mov qword [rsp+40], rax
;
;        ;Alineamiento de memoria
;;        mov qword rax, [iretq_registers + 32]
;;        mov qword [rsp + 48], rax
;
;        pop rax
;%endmacro
;%macro restoreRegs 1
;        push qword[%1 + 136]                ; Pushea los flags (solo se pueden restaurar los flags desde el stack)
;        popfq                               ; Restarura los flags
;        mov r8, qword[%1]
;    	mov r9, qword[%1+8]
;    	mov r10, qword[%1+16]
;    	mov r11, qword[%1+24]
;    	mov r12, qword[%1+32]
;    	mov r13, qword[%1+40]
;    	mov r14, qword[%1+48]
;    	mov r15, qword[%1+56]
;    	mov rax, qword[%1+64]
;    	mov rbx, qword[%1+72]
;    	mov rcx, qword[%1+80]
;    	mov rdx, qword[%1+88]
;    	mov rsi, qword[%1+96]
;    	mov rdi, qword[%1+104]
;    	mov rbp, qword[%1+112]
;        mov rsp, qword[%1+120]                      ; Esto es lo que hace el cambio de contexto, se mueve a otro punto del stack
;
;        push rax
;        push rbx
;        mov rbx, qword[off] ;guardo el offset de antes
;        mov rax, rsp ;el rsp nuevo
;        add rax, rbx ;le dejo la misma diferencia que en el stack que lo llama
;        ;lea rax, [rsp + 56] ;esta 48 + 8 (por rax) = 56 mas abajo
;        ;mov qword rax, [iretq_registers+16]
;        mov qword [rsp+40], rax ;guardo RSP 24+8+8
;        pop rbx
;
;
;
;
;        ;nota: gdb agrega en el stack un elemento que es la funcion donde esta el ususario en este momento
;        ;ese no es un elemento del stack, es solo para que el usuario sepa donde esta
;        ;Esto lo agrego por lo que vi de iretq
;        ;RIP
;        mov qword rax, [%1+128]                     ; Guardo en RAX el valor de RIP obtenido en saveRegs
;        mov qword [rsp+8], rax                      ; tengo que hacer rsp+8 por el push de rax
;
;        ;CS
;        mov qword rax, [iretq_registers]
;        mov qword [rsp+16],rax
;
;        ;FLAGS TODO: ver si este tiene que ser un valor especial para poder volver (Alejo dice que hay un flag que tiene que tener si esta en una interrupcion)
;        ;Ahora estoy dejando el valor del que crea el proceso en el caso de que este agregando
;        mov qword rax, [iretq_registers+8]
;        mov qword [rsp+24],rax
;
;
;        ;mov rax, rsp
;        ;sub rax, 48h ;va a ser 56 mas abajo del que estaba en el handler (para considerar a los cambios por el stack frame de la interrupcion)
;
;
;        ;SS
;        mov qword rax, [iretq_registers +32] ;restauro SS
;        mov qword [rsp+40], rax
;
;        ;Alineamiento de memoria
;;        mov qword rax, [iretq_registers + 32]
;;        mov qword [rsp + 48], rax
;
;        pop rax
;%endmacro
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
    ;cli
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
    jz sys_mem
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
;   rcx: vector con las direcciones de inicio de los programas (si son 2, el primero es el de la izquierda)
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: 0 si logro correr el programa; -1 si no
;------------------------------------------------------------------------------------
sys_exec:
    mov rdi, rbx
    mov rsi, rcx
    call exec_handler
    jmp fin

;-------------------------------------------------------------------------------------
; Exit: Termina la ejecucion del proceso que la llama
;-------------------------------------------------------------------------------------
; Parametros:
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

fin:
    mov [aux], rax                                  ; Resguardamos el valor de retorno (ojo que aux es una direccion, y queremos guardar adentro)
    restoreRegs curr_context                        ; Recuperamos los registros
    mov rax, [aux]                                  ; Recuperamos el valor de retorno en rax
    ;sti
    iretq




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

get_registers:
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
	exc_state resb 152		    ; Guarda 8*18 lugares de memoria (para los 18 registros)
	;exc_state es distinto a curr_contex porque en el primero guardo el rsp de donde ocurre la excepcion y no de cuando llega al handler
	curr_context resb 152       ; Contexto del programa (para almacenar procesos)
	;TODO: cambiar tamaño
	iretq_registers resb 144         ; Auxiliar para guardar otras cosas que deja iretq
	aux resq 1                  ; Variable auxiliar
	off resq 1

SECTION .data
	r8_o equ 0
	r9_o equ 8
	r10_o equ 16
	r11_o equ 24
	r12_o equ 32
	r13_o equ 40
	r14_o equ 48
	r15_o equ 56
	rax_o equ 64
	rbx_o equ 72
	rcx_o equ 80
	rdx_o equ 88
	rsi_o equ 96
	rdi_o equ 104
	rbp_o equ 112
	rsp_o equ 120
	rip_o equ 128
	rflags_o equ 136
	a_rflags_o equ 144