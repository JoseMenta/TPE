#ifndef _TIME_H_
#define _TIME_H_

void timer_handler();                       // Es la funcion que se ejecuta cuando ocurra la interrupcion del timer tick (Incrementa la cantidad de ticks)
int ticks_elapsed();                        // Devuelve la cantidad de veces que se ejecuto la interrupcion del timer tick desde que se inicio el sistema
int seconds_elapsed();                      // Devuelve la cantidad de segundos que transcurrieron desde que se inicio el sistema (Se realiza un tick cada 18,2 segs)

#endif
