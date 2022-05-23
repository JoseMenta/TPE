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
extern void zero_division_exc();
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

    print_lines();
//    ncPrint("Funciona?");
    //Testeos del driver que hicimos
    //clear(LEFT);
//    print("Hola",WHITE,RIGHT);
//    clear(ALL);//borra toda la pantalla!!!
//println("PARA TODOS dahjfhajdfhkjfhdajkfhdkjfhdkjahfdkjhdafkjdhjkdafhdjkfhakjhakahfjkahfkjdfhkjdahdfkjhkjfhdakjdf", YELLOW, ALL);
//    println("Hola dahjfhajdfhkjfhdajkfhdkjfhdkjahfdkjhdafkjdhjkdafhdjkfhakjhakahfjkahfkjdfhkjdahdfkjhkjfhdakjdf",RED,LEFT);
//    scroll_up(LEFT);
//    println("Hola dahjfhajdfhkjfhdajkfhdkjfhdkjahfdkjhdafkjdhjkdafhdjkfhakjhakahfjkahfkjdfhkjdahdfkjhkjfhdakjdf",WHITE,RIGHT);
//    scroll_up(RIGHT);
//    scroll_up(RIGHT);
//    scroll_up(RIGHT);
//    println("BUENAS",WHITE,RIGHT);
//    print("Abajo de buenas a la derecha",WHITE,RIGHT);
//    print("Abajo a la izquierda",WHITE,LEFT);
//    clear(LEFT);
//    print("Despues de clear",WHITE,LEFT);
//    println("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Amet dictum sit amet justo donec enim diam. Vitae elementum curabitur vitae nunc sed. At consectetur lorem donec massa sapien. Lacus vel facilisis volutpat est velit egestas dui id. Odio pellentesque diam volutpat commodo. Sed risus ultricies tristique nulla aliquet enim tortor. Platea dictumst quisque sagittis purus. Quis commodo odio aenean sed. Aliquam purus sit amet luctus venenatis lectus magna fringilla urna. Ornare suspendisse sed nisi lacus sed viverra tellus. Proin sagittis nisl rhoncus mattis rhoncus. Pellentesque nec nam aliquam sem. Velit egestas dui id ornare. Interdum posuere lorem ipsum dolor sit amet consectetur adipiscing. Purus in massa tempor nec feugiat nisl pretium. Aenean euismod elementum nisi quis eleifend quam. Nibh mauris cursus mattis molestie. Faucibus turpis in eu mi bibendum neque. Feugiat in ante metus dictum at. Urna cursus eget nunc scelerisque viverra mauris in aliquam. Ipsum nunc aliquet bibendum enim facilisis gravida neque convallis a. Enim nulla aliquet porttitor lacus luctus accumsan. Lectus vestibulum mattis ullamcorper velit sed ullamcorper morbi. Vitae semper quis lectus nulla at volutpat. Hendrerit gravida rutrum quisque non. Viverra maecenas accumsan lacus vel facilisis. Amet est placerat in egestas erat imperdiet sed euismod. Dui nunc mattis enim ut. Malesuada pellentesque elit eget gravida cum sociis natoque. Tellus orci ac auctor augue mauris. Sed enim ut sem viverra aliquet eget. Venenatis a condimentum vitae sapien pellentesque habitant morbi. Suspendisse in est ante in nibh mauris. Pulvinar sapien et ligula ullamcorper malesuada proin libero. Eros in cursus turpis massa tincidunt dui ut. Neque egestas congue quisque egestas. Tortor condimentum lacinia quis vel eros donec. Aliquet sagittis id consectetur purus ut faucibus pulvinar elementum. Nulla at volutpat diam ut. Urna nunc id cursus metus aliquam. Viverra nibh cras pulvinar mattis. Leo vel fringilla est ullamcorper eget nulla facilisi etiam. Congue quisque egestas diam in arcu cursus. Pulvinar mattis nunc sed blandit libero volutpat sed cras. Risus ultricies tristique nulla aliquet enim tortor at euctor.",WHITE,LEFT);
//
//    println("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Amet dictum sit amet justo donec enim diam. Vitae elementum curabitur vitae nunc sed. At consectetur lorem donec massa sapien. Lacus vel facilisis volutpat est velit egestas dui id. Odio pellentesque diam volutpat commodo. Sed risus ultricies tristique nulla aliquet enim tortor. Platea dictumst quisque sagittis purus. Quis commodo odio aenean sed. Aliquam purus sit amet luctus venenatis lectus magna fringilla urna. Ornare suspendisse sed nisi lacus sed viverra tellus. Proin sagittis nisl rhoncus mattis rhoncus. Pellentesque nec nam aliquam sem. Velit egestas dui id ornare. Interdum posuere lorem ipsum dolor sit amet consectetur adipiscing. Purus in massa tempor nec feugiat nisl pretium. Aenean euismod elementum nisi quis eleifend quam. Nibh mauris cursus mattis molestie. Faucibus turpis in eu mi bibendum neque. Feugiat in ante metus dictum at. Urna cursus eget nunc scelerisque viverra mauris in aliquam. Ipsum nunc aliquet bibendum enim facilisis gravida neque convallis a. Enim nulla aliquet porttitor lacus luctus accumsan. Lectus vestibulum mattis ullamcorper velit sed ullamcorper morbi. Vitae semper quis lectus nulla at volutpat. Hendrerit gravida rutrum quisque non. Viverra maecenas accumsan lacus vel facilisis. Amet est placerat in egestas erat imperdiet sed euismod. Dui nunc mattis enim ut. Malesuada pellentesque elit eget gravida cum sociis natoque. Tellus orci ac auctor augue mauris. Sed enim ut sem viverra aliquet eget. Venenatis a condimentum vitae sapien pellentesque habitant morbi. Suspendisse in est ante in nibh mauris. Pulvinar sapien et ligula ullamcorper malesuada proin libero. Eros in cursus turpis massa tincidunt dui ut. Neque egestas congue quisque egestas. Tortor condimentum lacinia quis vel eros donec. Aliquet sagittis id consectetur purus ut faucibus pulvinar elementum. Nulla at volutpat diam ut. Urna nunc id cursus metus aliquam. Viverra nibh cras pulvinar mattis. Leo vel fringilla est ullamcorper eget nulla facilisi etiam. Congue quisque egestas diam in arcu cursus. Pulvinar mattis nunc sed blandit libero volutpat sed cras. Risus ultricies tristique nulla aliquet enim tortor at iuctor.",WHITE,RIGHT);
//
    //scroll_up(RIGHT);
//	print("dahjfhajdfhkjfhdajkfhdkjfhdkjahfdkjhdafkjdhjkdafhdjkfhakjhakahfjkahfkjdfhkjdahdfkjhkjfhdakjdf", BLUE, ALL);
//	new_line(ALL);
//	print("dahjfhajdfhkjfhdajkfhdkjfhdkjahfdkjhdafkjdhjkdafhdjkfhakjhakahfjkahfkjdfhkjdahdfkjhkjfhdakjdf", BLUE, ALL);
//    new_line(ALL);
//	new_line(RIGHT);
//    print("Abajo de Hola a la derecha",WHITE,RIGHT);
//	new_line(ALL);
//    new_line(LEFT);
//    print("Abajo de Buenas a la izquierda",WHITE,LEFT);
// Para probar si fuinciona bien el next_left
//    print("1bcdefghijklmnopqrstuvwxyz1234567890ABC2bcdefghijklmnopqrstuvwxyz1234567890ABC3bcdefghijklmnopqrstuvwxyz1234567890ABC4bcdefghijklmnopqrstuvwxyz1234567890ABC5bcdefghijklmnopqrstuvwxyz1234567890ABC6bcdefghijklmnopqrstuvwxyz1234567890ABC7bcdefghijklmnopqrstuvwxyz1234567890ABC8bcdefghijklmnopqrstuvwxyz1234567890ABC9bcdefghijklmnopqrstuvwxyz1234567890ABC10cdefghijklmnopqrstuvwxyz1234567890ABC11cdefghijklmnopqrstuvwxyz1234567890ABC12cdefghijklmnopqrstuvwxyz1234567890ABC13cdefghijklmnopqrstuvwxyz1234567890ABC14cdefghijklmnopqrstuvwxyz1234567890ABC15cdefghijklmnopqrstuvwxyz1234567890ABC16cdefghijklmnopqrstuvwxyz1234567890ABC17cdefghijklmnopqrstuvwxyz1234567890ABC18cdefghijklmnopqrstuvwxyz1234567890ABC19cdefghijklmnopqrstuvwxyz1234567890ABC20cdefghijklmnopqrstuvwxyz1234567890ABC21cdefghijklmnopqrstuvwxyz1234567890ABC22cdefghijklmnopqrstuvwxyz1234567890ABC23cdefghijklmnopqrstuvwxyz1234567890ABC24cdefghijklmnopqrstuvwxyz1234567890ABC25cdefghijklmnopqrstuvwxyz1234567890ABC26cdefghijklmnopqrstuvwxyz1234567890ABC27cdefghijklmnopqrstuvwxyz1234567890ABC",WHITE,LEFT);
//    print("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccccccccccccccc", WHITE, RIGHT);
//    print("Hola", BLUE, LEFT);
//    delete_last_char(LEFT);
//    delete_last_char(RIGHT);
//    delete_last_char(RIGHT);
//    print("Chau", RED, LEFT);
//    print("QUE", YELLOW, RIGHT);
    zero_division_exc();
    //  print("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.",WHITE,LEFT);
  //  print("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.",WHITE,RIGHT);
        //    clear(RIGHT);
//    //clear(ALL);
//    print("Hola", WHITE, ALL);
//    new_line(ALL);
//    print("Hola en rojo", RED, ALL);
//    new_line(ALL);
//    print("Hola en azul", BLUE, ALL);
//    new_line(ALL);
//    print("Hola por izq", WHITE, LEFT);
//    print("Hola por der", WHITE, RIGHT);
//    new_line(LEFT);
//    new_line(RIGHT);
//    print("Hola por izq rojo", RED, LEFT);
//    print("Hola por der azul", BLUE, RIGHT);
//    new_line(ALL);
//    print_char('a', WHITE, ALL);
//    new_line(ALL);
//    print_char('b', RED, ALL);
//    new_line(ALL);
//    print_char('c', BLUE, ALL);
//    new_line(ALL);
//    print_char('l', WHITE, LEFT);
//    print_char('r', WHITE, RIGHT);
//    new_line(LEFT);
//    new_line(RIGHT);
//    print_char('L', RED, LEFT);
//    print_char('R', BLUE, RIGHT);
//    clear(ALL);
//    print("CAMBIO A INTERUPCIONES CON int 80h:", WHITE, ALL);
//    TesterWrite();
//    TesterRead();
	return 0;
}
