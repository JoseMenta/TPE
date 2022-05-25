GLOBAL cpuVendor
GLOBAL default_process
GLOBAL get_time

section .text
	
cpuVendor:							; Devuelve un string del nombre del fabricante del procesador (por ejemplo, si es Intel devuelve el string "Genuine Intel")
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

default_process:
    ;mov rax, 1 ;no se si necesita tener esta instruccion, probar despues
    nop
    jmp default_process ;usa al procesador y no cambia las cosas


;-------------------------------------------------------------------------------------
; get_time: Devuelve una unidad de tiempo (segundo, minuto, hora, dia, mes o año) segun el parametro en GMT
;-------------------------------------------------------------------------------------
; Parametros:
;   rdi: tipo de unidad esperada (0 = secs, 2 = mins, 4 = hour, 7 = day, 8 = month, 9 = year)
;-------------------------------------------------------------------------------------
; Retorno:
;   rax: valor para el tipo de unidad indicado
;------------------------------------------------------------------------------------
get_time:
	push rbp
	mov rbp, rsp			; Armado de stack frame

	; Obtengo el registro OBh (Registro de Control)
    mov al, 0Bh
    out 70h, al
    in al, 71h


    ; Creacion de la mascara para poner en 1 el bit 2 (tercero) para leer el tiempo en binario
    mov bl, 4h
    or al, bl

	push rax

	; Vuelvo a obtener el registro OBh
    mov al, 0Bh
    out 70h, al

	pop rax

	; Modifico el registro
    out 71h, al

    ; Obtenemos los segundos, minutos u hora (segun el codigo recibido por parametro) en el registro AL y se retorna
    mov rax, rdi
    out 70h, al
    in al, 71h

	mov rsp, rbp
	pop rbp					; Desarmado de stack frame
	ret
