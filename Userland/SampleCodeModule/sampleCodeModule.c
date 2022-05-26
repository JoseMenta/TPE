/* sampleCodeModule.c */
#include <libc.h>
#include <tests.h>
#include <programs.h>

char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;


int main() {
	//All the following code may be removed 
//	*v = 'X';
//	*(v+1) = 0x74;

	//Test if BSS is properly set up: Es decir que se ha limpiado el segmento BSS en _loader.c
	//if (var1 == 0 && var2 == 0)
		//return 0xDEADC0DE;
    //return 0xDEADBEEF;
//    sys_write("holaaaa UserSpace", WHITE);
    //zero_division_exc();

    void * aux[1] = {write_test};
    sys_exec(1, aux);
    (uint64_t) write_test;
     return 0;

}