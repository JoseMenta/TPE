/* sampleCodeModule.c */
#include <libc.h>
#include <tests.h>
#include <programs.h>

char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;


int main() {
//    uint64_t main_rip = (uint64_t)&main; //esto tiene la direccion de RIP cuando empieza main
//    uint64_t main_rip_2 = (uint64_t)main;
//    //Son equivalentes las 2 de arriba
//    void* main_rip_3 = main; //Estas las guardan como un hexa (es lo mismo que antes, pero gdb lo muestra distinto por el tipo)
//    void* main_rip_4 = &main;
    //Son equivalentes las 2 de arriba
    //Dan 40007d cuando las compile
	//All the following code may be removed 
//	*v = 'X';
//	*(v+1) = 0x74;

	//Test if BSS is properly set up: Es decir que se ha limpiado el segmento BSS en _loader.c
	//if (var1 == 0 && var2 == 0)
		//return 0xDEADC0DE;
    //return 0xDEADBEEF;
//    sys_write("holaaaa UserSpace", WHITE);
    //zero_division_exc();

    void * aux[1] = {read_test};
    sys_exec(1, aux);
    //(uint64_t) write_test;
    sys_write("fin",WHITE);
     return 0;

}