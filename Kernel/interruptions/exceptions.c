#include <naiveConsole.h>
#include <stdint.h>

void zero_division();
void invalid_opcode();

typedef void (* exception) ();

exception exceptions[2] = {zero_division, invalid_opcode};              // Arreglo de punteros a funcion de excepciones

uint64_t* getRegisters(void);
void printRegisters();

static const char * Names[18] = { "R8: ", "R9: ", "R10: ", "R11: ", "R12: ", "R13: ", "R14: ", "R15: ", "RAX: ", "RBX: ", "RCX: ", "RDX: ", "RSI: ", "RDI: ", "RBP: ", "RSP: ", "RIP: ", "FLAGS: "};

void exceptionDispatcher(int exception) {
	exceptions[exception]();
	return;
}

// Esta funcion es la que se ejecutara al realizarse una division por cero (exceptionID = 0)
void zero_division() {
	ncPrint("Excepcion generada: Division por cero");		//ponerlo bien con el sys_write
	printRegisters();
}
/*
programa a llamar desde el Userland cuando quiero la excepcion
void zero_division() {
	zero_division_exc();
}

Funcion en ASM que genera la excepcion
zero_division_exc:
	push rbp
	mov rbp, rsp

	int 0h
	
	mov rsp, rbp
	pop rbp
	ret
*/


// Esta funcion es la que se ejecutara al utilizar un operador invalido (exceptionID = 6)
void invalid_opcode() {
	ncPrint("Excepcion generada: Invalid opcode");		//ponerlo bien con el sys_write
	printRegisters();
}
/*
programa a llamar desde el Userland cuando quiero la exepcion
void invalid_opcode() {
	invalid_opcode_exc();
}

Funcion en ASM que genera la excepcion
invalid_opcode_exc:
	push rbp
	mov rbp, rsp

	int 6h
	
	mov rsp, rbp
	pop rbp
	ret
*/

void printRegisters(){
	ncPrint("Registros al momento de la excepcion: ");
	//pongo en reg los valores de los registros
	uint64_t* reg = getRegisters();
	for(int i=0; i<18; i++){
		ncPrint(Names[i]);				//ponerlo bien con el sys_write
		ncPrintHex(reg[i]);
	}
}