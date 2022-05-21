#include <video_driver.h>
#include <stdint.h>
#define NEXT 2
#define WIDTH  2*80
#define HEIGHT 25
#define MID_WIDTH 2*39 //El ancho de media pantalla
#define RIGHT_LAST_COLUMN = 2*79
#define LEFT_LAST_COLUMN = MID_WIDTH
//Se hacen 2 lineas en el medio
#define L_START = 0
#define R_START = (WIDTH-MID_WIDTH)
#define ALL_START = L_START
#define L_END (WIDTH * HEIGHT - (2*(WIDTH - MID_WIDTH))) //fin de la pantalla izquierda (extremo inferior derecho)
#define R_END (WIDTH * HEIGHT -NEXT) //fin de la pantalla derecha
#define ALL_END = R_END
uint8_t left_next(uint8_t index);
uint8_t right_next(uint8_t index);
uint8_t all_next(uint8_t index);
static uint8_t* const video_start = (uint8_t *) 0xB8000;
//static uint8_t* curr_video = (uint8_t *) 0xB8000;
//Usamos un offset para manerjarnos en la pantalla
//Es decir, la direccion donde se tiene que escribir va a ser video_start + left_offset|right_offset|all_offset
static uint8_t left_offset= L_START;
static uint8_t right_offset = R_START;
static uint8_t all_offset = ALL_START;
//-----------------------------------------------------------------------
// video_print_char: imprime un caracter en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -character: el caracter que se quiere imprimir
// -color: el color de la letra, una constante de color definida en .h
// -background: el color de fondo, una constante de color definida en .h
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void printChar(char c, int letterFormat, positionType position){
    uint8_t* curr = video_start; //La primera direccion de video
    //A esto se le suma el offset correspondiente
    if(position==LEFT){
        curr += left_offset;
        if(left_offset == L_END){
            //tengo que hacer scroll up
            scroll_up(position);
        }
        left_offset = left_next(left_offset);

    }else if (position==RIGHT){
        curr += right_offset;
        if(right_offset == R_END){ //llegue al final de la pantalla
            //tengo que hacer scroll up
            scroll_up(position);
        }
        right_offset = right_next(right_offset);
    }else{
        //position == ALL
        curr += all_offset;
        if(all_offset == ALL_END){
            //Tengo que hacer scroll up
            scroll_up(position);
        }
        all_offset = all_next(all_offset);
    }
    if(c == '\n'){
        new_line(position);
    }else {
        //Imprime el caracter en la posicion indicada
        *curr_video = c;//imprimo el ASCII
        *(curr_video + 1) = letterFormat; //indico el color que va a tener
    }
}
//-----------------------------------------------------------------------
// print: imprime un string en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -str: el string que se quiere imprimir
// -letterFotmat: el color de la letra, una constante de color definida en .h
// -position: la posicion de la pantalla donde se desea imprimir
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print(char * str, int letterFormat, positionType position){
    for(;*str!='\0';str++){
        printChar(*str,letterFormat,position);
    }
}
//-----------------------------------------------------------------------
// println: imprime un string en pantalla y salta a la proxima linea
//-----------------------------------------------------------------------
// Argumentos:
// -str: el caracter que se quiere imprimir
// -letterFormat: el color de la letra, una constante de color definida en .h
// -position: posicion de la pantalla donde se quiere imprimir
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a scroll_up
//-----------------------------------------------------------------------
void println(char * str, int letterFormat, positionType position){
    print(str,letterFormat,position);
    new_line(position);
}
//-----------------------------------------------------------------------
// new_line: ubica al cursor en la proxima linea
//-----------------------------------------------------------------------
// Argumentos:
// -position: La posicion de la pantalla donde se desea ir a la proxima linea
//-----------------------------------------------------------------------
void new_line(positionType position){
    if(position == LEFT){
        left_offset += (MID_WIDTH - left_offset%MID_WIDTH);//va al final de la pantalla izquierda
        left_offset += (WIDTH - MID_WIDTH);//voy al final de la pantalla
        left_offset += NEXT; //va a la primera posicion de la siguiente fila
    }else if(position == RIGHT){
        right_offset += (MID_WIDTH- right_offset%MID_WIDTH);//va al final de la fila en la pantalla derecha
        right_offset += (WIDTH - MID_WIDTH); //va a la posicion de la segunda linea (en la siguiente fila)
        right_offset += NEXT; //va a la primera posicion de la siguiente fila de la pantalla derecha
    }else{
        //position == ALL
        all_offset += (WIDTH - all_offset % WIDTH); //voy al final de la pantalla
        all_offset += NEXT; //va a la primera posicion de la fila siguiente

    }

}
//-----------------------------------------------------------------------
// Funciones auxiliares para obtener la siguiente posicion donde tiene que ir el offset dependiendo del caso
//-----------------------------------------------------------------------
uint8_t left_next(uint8_t index){
    if(index%WIDTH==LEFT_LAST_COLUMN){//Llegue al final de la pantalla
        index += (WIDTH - MID_WIDTH);//voy al final de la pantalla total (en la misma fila)
    }
    index+=NEXT; //voy al principio de la siguiente fila
    return index;
}
uint8_t right_next(uint8_t index){
    if(index%WIDTH==RIGHT_LAST_COLUMN){
        index+= (WIDTH - MID_WIDTH); //voy al final de la segunda linea (en la fila de abajo)
    }
    index+=NEXT;
    return index;
}
uint8_t all_next(uint8_t index){
    return index+NEXT;//voy al la siguiente posicion de la pantalla
}
//Todo: implementar
void scroll_up(positionType position){
    if(position == LEFT){

    }else if (position == RIGHT){

    }else{

    }
    for(uint8_t* i = video_start; i<curr_video; i++){
        *i = *(i+2*WIDTH);
        //copio la linea actual a la de arriba
    }
    if(curr_video>=video_start+(WIDTH*2)){
        //si no estoy en la primera linea, tengo que llenarlo con
        //espacios y moverme hacia arriba
        for(uint8_t* i = curr_video-(2*WIDTH);i<=curr_video;i+=2){
            *i = ' ';
        }
        //intenta sacar esto jajaja
        curr_video-=(2*WIDTH);
    }else{
        curr_video = video_start;
        //si estoy en la primera linea, borro todo y vuelvo al principio
    }
}
//-----------------------------------------------------------------------
// clear: limpia la pantalla, pone ' ' en todos los lugares disponibles
//-----------------------------------------------------------------------
// Argumentos:
// -position: La posicion de la pantalla que se desea limpiar
//-----------------------------------------------------------------------
void clear(positionType position){
    if(position == LEFT){
        for(uint8_t l = L_START; l<=L_END; l = left_next(l)){
            *(video_start+l) = ' ';
            *(video_start + l + 1) = 0;
        }
        left_offset = L_START;
    }else if(position == RIGHT){
        for(uint8_t r = R_START; r<=R_END; r = right_next(r)){
            *(video_start+r) = ' ';
            *(video_start + r + 1) = 0;
        }
        right_offset = R_START;
    }else{
        for(uint8_t i = ALL_START; i<=ALL_END; i = all_next(i)){
            *(video_start + i) = ' ';
            *(video_start + i + 1) = 0;
        }
        all_offset = ALL_START;
    }
}
