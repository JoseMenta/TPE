#include <video_driver.h>

#define WIDTH  2*80
#define HEIGHT 25
#define MID_WIDTH 2*39 //El ancho de media pantalla
#define RIGHT_LAST_COLUMN = 2*79
#define LEFT_LAST_COLUMN = MID_WIDTH
//Se hacen 2 lineas en el medio
#define L_END (WIDTH * HEIGHT - (2*41)) //fin de la pantalla izquierda (extremo inferior derecho)
#define R_END (WIDTH * HEIGHT -2) //fin de la pantalla derecha
static uint8_t* const video_start = (uint8_t *) 0xB8000;
//static uint8_t* curr_video = (uint8_t *) 0xB8000;
//Usamos un offset para manerjarnos en la pantalla
//Es decir, la direccion donde se tiene que escribir va a ser video_start + left_offset|right_offset|all_offset
static uint8_t left_offset= 0;
static uint8_t right_offset = 0;
static uint8_t all_offset = 0;
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
    uint8_t* curr = video_start;
    if(position==LEFT){
        curr += left_offset;
        if(left_offset == L_END){
            //tengo que hacer scroll up
            scroll_up(position);
        }
        if(left_offset%WIDTH==LEFT_LAST_COLUMN){//Llegue al final de la pantalla
            left_offset += (WIDTH - MID_WIDTH);//voy al final de la pantalla total
        }
        left_offset+=2;
    }else if (position==RIGHT){
        curr += right_offset;
        if(right_offset == R_END){ //llegue al final de la pantalla
            //tengo que hacer scroll up
            scroll_up(position);
        }
        if(right_offset%WIDTH==RIGHT_LAST_COLUMN){
            right_offset+=(WIDTH - MID_WIDTH); //voy al final de la segunda linea
        }
        right_offset+=2;
    }else{
        //position == ALL
        curr += all_offset;
        if(all_offset == R_END){
            //Tengo que hacer scroll up
            scroll_up(position);
        }
        if(all_offset%WIDTH == RIGHT_LAST_COLUMN){
            all_offset+=(WIDTH - MID_WIDTH);
        }
        all_offset+=2;
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
// video_print_str: imprime un string en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -str: el string que se quiere imprimir
// -color: el color de la letra, una constante de color definida en .h
// -background: el color de fondo, una constante de color definida en .h
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print(char * str, int letterFormat, positionType position){
    for(;*str!='\0';str++){
        printChar(*str,letterFormat,position);
    }
}
//-----------------------------------------------------------------------
// video_println: imprime un string en pantalla y salta a la proxima linea
//-----------------------------------------------------------------------
// Argumentos:
// -str: el caracter que se quiere imprimir
// -color: el color de la letra, una constante de color definida en .h
// -background: el color de fondo, una constante de color definida en .h
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void println(char * str, int letterFormat, positionType position){
    print(str,letterFormat,position);
    new_line(position);
}
//-----------------------------------------------------------------------
// video_new_line: ubica al cursor en la proxima linea
//-----------------------------------------------------------------------
void video_new_line(){
    do{
        video_print_char(' ',0,0);
    }while((uint64_t) (curr_video-video_start) % (2*WIDTH) != 0);
}
//-----------------------------------------------------------------------
// video_scroll_up: sube todo lo que esta en pantalla una linea hacia arriba
//                  eliminando la primera linea
//-----------------------------------------------------------------------

void scroll_up(positionType position){
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
// video_clear: limpia la pantalla, pone ' ' en todos los lugares disponibles
//-----------------------------------------------------------------------

void video_clear(){
    for(int i = 0; i<HEIGHT*WIDTH; i++){
        video_start[2*i] = ' '; //lo lleno de espacios
        video_start[i*2+1] = 0; //le saco el color
    }
    curr_video = video_start;
}
