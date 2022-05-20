#include <time.h>
#include <stdint.h>
#include "../include/keyboard.h"
#include <naiveConsole.h>
static void int_20();
static void int_21();

typedef void (* handler) ();

handler handlers[2] = {int_20, int_21};              // Arreglo de punteros a funcion de interrupciones

void irqDispatcher(uint64_t irq) {
	//segun el parametro que tiro el IrqHandler cae en algun case
    //eso te lleva a la funcion posta donde defino que hace la irq
    handlers[irq]();
	return;
}

// La interrupcion 0x20 es la del timer tick -> Llamamos a la rutina de atencion del mismo
void int_20() {
	timer_handler();
}

void int_21(){
    keyboard_handler();
}
