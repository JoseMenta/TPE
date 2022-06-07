#ifndef IDTLOADER_H
#define IDTLOADER_H

//----------------------------------------------------------------------
// load_idt: carga las rutinas de atencion a interrupciones y excepciones
//----------------------------------------------------------------------
// En esta funcion se deben cargar las interrupciones y las excepciones que se desean utilizar, asociando el codigo de
// interrupcion/excepcion con su respectiva rutina de atencion
//----------------------------------------------------------------------
void load_idt();

#endif