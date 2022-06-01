#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {							// Es la funcion que se ejecuta cuando ocurra la interrupcion del timer tick (Incrementa la cantidad de ticks)
	ticks++;
    //if(ticks%2==0)
        change_context();
}

int ticks_elapsed() {							// Devuelve la cantidad de veces que se ejecuto la interrupcion del timer tick desde que se inicio el sistema
	return ticks;
}

int seconds_elapsed() {							// Devuelve la cantidad de segundos que transcurrieron desde que se inicio el sistema (Se realiza un tick cada 18,2 segs)
	return ticks / 18;
}
