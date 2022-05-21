#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <video_driver.h>
#include <scheduler.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss; 
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;						// El tamaño de una pagina (en este caso, se definio que sea de 4KB)
int curr_process = 0;
//0 es la terminal, que escribe en ALL, 
//Terminal llama a primos en toda la pantalla
// -> primos.process=1
//  primos.side = LEFT
//Si llama a |
//Desde terminal, con una syscall exec agrego a los 2 procesos
//

//Teclado 
// Int hardware -> guarde las teclas en un buffer
// Terminal lo que hace es llamar a sys_read, y eso solo lee el buffer
// La terminal recibe \n -> 
// Si no reconoce lo que escribieron -> Imprime error y vuelve a leer
// Si esta | => llama a exec con 2 programas y le dice donde va cada uno 
// SI no esta el | => llama a exec con 1 programa side  = ALL

// Exec(2)
// Solo hacer el arreglo para cuando hago |
// sys_write:
// 1- recibe un argumento de donde va
// 2- mira el arreglo con procesos (el arreglo solo se hace con |
// 1: Tengo que pasarle al programa donde esta
// primos(A) -> sys_write(A) -> sys_write ve que hace
// fib(B) -> sys_write(B)
// Si paso parametro -> primos(A) puede escribir en el lado B
// Es medio una falla de seguridad, 
// fib | primos
// Si no
// primos(A) -> primos ve si llama a sys_write_left o sys_write_right
// 2: Mira el arreglo
// El problema es cuando no hay procesos en el arreglo (cuando no haces |) -> 

// sys_write(L) en primos
// Primos tiene una excepcion 
// Donde escribe la excepcion?
// Como sabe la excepcion que esta en L?

//struct process{
//	int pid;
//	struct registers;
//	char side; //LEFT,RIGHT,ALL para
//	int status; //RUNNING,PAUSED,...
//};
static void * const sampleCodeModuleAddress = (void*)0x400000;	// Userland comienza en la direccion 0x400000
static void * const sampleDataModuleAddress = (void*)0x500000;

extern void TesterWrite();
extern void TesterRead();

typedef int (*EntryPoint)();									// Entrypoint es un alias para punteros a funcion que devuelven un entero, sin parametros


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()											// Devuelve el valor del registro RSP (direccion de memoria donde apunta el registro)
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()													// Es la primera funcion que se ejecutará una vez se halla cargado el SO en el sistema
{
    load_idt();
	ncPrint("[Kernel Main]");
	ncNewline();
	ncPrint("  Sample code module at 0x");
	ncPrintHex((uint64_t)sampleCodeModuleAddress);
	ncNewline();
	ncPrint("  Calling the sample code module returned: ");
	ncPrintHex(((EntryPoint)sampleCodeModuleAddress)());
	ncNewline();
	ncNewline();

	ncPrint("  Sample data module at 0x");
	ncPrintHex((uint64_t)sampleDataModuleAddress);
	ncNewline();
	ncPrint("  Sample data module contents: ");
	ncPrint((char*)sampleDataModuleAddress);
	ncNewline();

	ncPrint("[Finished]");
    ncClear();

    //Testeos del driver que hicimos
    clear(LEFT);
    clear(RIGHT);
    clear(ALL);
    print("Hola", WHITE, ALL);
    new_line(ALL);
    print("Hola en rojo", RED, ALL);
    new_line();
    print("Hola en azul", BLUE, ALL);
    new_line(ALL);
    print("Hola por izq", WHITE, LEFT);
    print("Hola por der", WHITE, RIGHT);
    Newline(LEFT);
    new_line(RIGHT)
    print("Hola por izq rojo", RED, LEFT);
    print("Hola por der azul", BLUE, RIGHT);
    new_line(ALL);
    print_char('a', WHITE, ALL);
    new_line(ALL);
    print_char('b', RED, ALL);
    new_line();
    print_char('c', BLUE, ALL);
    new_line(ALL);
    print_char('l', WHITE, LEFT);
    print_char('r', WHITE, RIGHT);
    new_line(LEFT);
    new_line(RIGHT);
    print_char('L', RED, LEFT);
    print_char('R', BLUE, RIGHT);
    clear(ALL);
    print("CAMBIO A INTERUPCIONES CON int 80h:", WHITE, ALL);
    TesterWrite();
    TesterRead();
	return 0;
}
