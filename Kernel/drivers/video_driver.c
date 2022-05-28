
#include <video_driver.h>
#include <stdint.h>

#define WIDTH 160
#define HEIGHT 25
#define NEXT_COL 2
#define MID_WIDTH (WIDTH/2)

#define L_ROW_START 0
#define L_COL_START 0

#define L_ROW_END (HEIGHT - 1)
#define L_COL_END (MID_WIDTH - 2 * NEXT_COL)

#define R_ROW_START 0
#define R_COL_START (MID_WIDTH + NEXT_COL)

#define R_ROW_END (HEIGHT - 1)
#define R_COL_END (WIDTH - NEXT_COL)

#define A_ROW_START 0
#define A_COL_START 0

#define A_ROW_END (HEIGHT - 1)
#define A_COL_END (WIDTH - NEXT_COL)

#define SCREEN_ENDED(pos) ((pos).row_current == (pos).row_end && (pos).col_current == (pos).col_end)
#define VIDEO_OFFSET(pos) (WIDTH*((pos).row_current) + (pos).col_current)

//TODO: sacar magic numbers
typedef struct{
    uint32_t row_start;             // La fila inicial
    uint32_t col_start;             // La columna inicial
    uint32_t row_current;           // La fila actual
    uint32_t col_current;           // La columna actual
    uint32_t row_end;               // La fila final
    uint32_t col_end;               // La columna final
} coordinatesType;

void next(coordinatesType * position);
void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position);

coordinatesType coordinates[3] = {  {A_ROW_START, A_COL_START, A_ROW_START, A_COL_START, A_ROW_END, A_COL_END},
                                    {L_ROW_START, L_COL_START, L_ROW_START, L_COL_START, L_ROW_END, L_COL_END},
                                    {R_ROW_START, R_COL_START, R_ROW_START, R_COL_START, R_ROW_END, R_COL_END} };

// Con estas variables podemos calcular la posicion donde se debe imprimir un caracter con la formula:
//                              video_start + WIDTH * row + column

//uint8_t para que cuando hacemos ++, vaya avanzando de a 1 byte
static uint8_t * const video_start = (uint8_t *) 0xB8000;


//-----------------------------------------------------------------------
// print_char: imprime un caracter en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -c: el caracter que se quiere imprimir
// -letterFormat: el color de la letra, una constante de color definida en .h
// -position: Indica en que posicion se debe imprimir el caracter
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print_char(char c, formatType letterFormat, positionType position){
    uint8_t * curr = video_start;                   // La primera direccion de video
    // A esto se le suma el offset correspondiente
    curr += VIDEO_OFFSET(coordinates[position]);
    // Luego, imprimimos el caracter (si es \n hara un salto de linea)
    print_aux(curr, c, letterFormat, position);
    if(SCREEN_ENDED(coordinates[position])){
        // Si llego al final de la pantalla, tengo que hacer scroll up
        scroll_up(position);
    }
    // Obtenemos la proxima posicion sobre la cual imprimir en la porcion correspondiente
    if(c!='\n') {
        next(&coordinates[position]);
    }
}

void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position){
    if(c == '\n'){
        new_line(position);
    }else{
        //Imprime el caracter en la posicion indicada
        *curr = c;                      // Imprimo el ASCII
        *(curr + 1) = letterFormat;     // Indico el color que va a tener
    }
}

//-----------------------------------------------------------------------
// print: imprime un string en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -str: el string que se quiere imprimir
// -letterFormat: el color de la letra, una constante de color definida en .h
// -position: la posicion de la pantalla donde se desea imprimir
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print(const char * str, formatType letterFormat, positionType position){
//    if(position!=LEFT && position!=RIGHT && position!=ALL) {
//        char aux[] = "error con position";
//        for(int i = 0; aux[i]!='\0';i++){
//            print_char(aux[i],RED,LEFT);
//        }
//    }
    for(;*str!='\0';str++){
        print_char(*str,letterFormat,position);
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
void println(const char * str, formatType letterFormat, positionType position){
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
    if(coordinates[position].row_current==coordinates[position].row_end){
        scroll_up(position);
    }
    // Posicionamos la fila de la porcion correspondiente en la proxima
    coordinates[position].row_current += 1;
    // Posicionamos la columna de la porcion correspondiente al principio
    coordinates[position].col_current = coordinates[position].col_start;
}

//-----------------------------------------------------------------------
// Funcion auxiliar para obtener la siguiente posicion donde tiene que ir el offset según la porcion indicado por parametro
//-----------------------------------------------------------------------

void next(coordinatesType * position){
    // Si se llego al final de una fila, paso a la proxima fila y al principio de ella
    if(position->col_current == position->col_end){
        position->col_current = position->col_start;
        position->row_current++;
    }
        // Si no llegue al final, paso a la siguiente columna en la fila
    else {
        position->col_current += NEXT_COL;
    }
}

//-----------------------------------------------------------------------
// scroll_up: Mueve los caracteres una linea arriba (la primera se pierde)
//-----------------------------------------------------------------------
// Argumentos:
// - position: la posicion de la pantalla donde se desea imprimir
//-----------------------------------------------------------------------
void scroll_up(positionType position){
    // Copiamos los caracteres de la fila n en n-1, n > 1
//    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
//        c.row_current != coordinates[position].row_current-1 || c.col_current != coordinates[position].col_current;
//        next(&c)){
//        *(video_start + c.row_current * WIDTH + c.col_current) = *(video_start + (c.row_current+1) * WIDTH + c.col_current);
//        *(video_start + c.row_current * WIDTH + c.col_current + 1) = *(video_start + (c.row_current+1) * WIDTH + c.col_current + 1);
//    }
//TODO: revisar esto, si no cuando hacemos (c.row_current+1) podemos caer en cualquier lado si coordinates[position].row_current==coordinates[position].row_end
    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
        c.row_current < coordinates[position].row_end;
        next(&c)){
        *(video_start + c.row_current * WIDTH + c.col_current) = *(video_start + (c.row_current+1) * WIDTH + c.col_current);
        *(video_start + c.row_current * WIDTH + c.col_current + 1) = *(video_start + (c.row_current+1) * WIDTH + c.col_current + 1);
    }
    // Nos paramos en la fila anterior pues todo se subio una fila arriba
    if(coordinates[position].row_current >= 1){
        // Si habia llegado a la ultima fila, debo borrar los caracteres en esa fila
        if(coordinates[position].row_current == coordinates[position].row_end){
            for(coordinatesType c = {coordinates[position].row_end, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
                c.row_current <= coordinates[position].row_end && c.col_current <= coordinates[position].col_end;
                next(&c)){
                *(video_start + c.row_current * WIDTH + c.col_current) = ' ';
                *(video_start + c.row_current * WIDTH + c.col_current + 1) = 0;
            }
        }
        coordinates[position].row_current--;
    }
        // En cambio, si estoy en la primera fila, se borro todo, por lo que se debe empezar desde el principio
    else {
        coordinates[position].col_current = coordinates[position].col_start;
    }
}

// ----------------------------------------------------------------------------
// print_lines: Imprime las lineas de separacion para cuando el usuario haga la función |
// ----------------------------------------------------------------------------
// Argumentos:
//  null
// ----------------------------------------------------------------------------
// Retorno:
//  null
// ----------------------------------------------------------------------------
void print_lines(){
    uint32_t col1 = 80;                         // La posicion 78 y 80 son la mitad de la pantalla en el arreglo del driver
    uint32_t col2 = 78;
    for(uint32_t i = 0; i <= A_ROW_END; i++){
        *(video_start + WIDTH * i + col1) = '|';
        *(video_start + WIDTH * i + col1 + 1) = WHITE;
        *(video_start + WIDTH * i + col2) = '|';
        *(video_start + WIDTH * i + col1 + 1) = WHITE;
    }
}

//-----------------------------------------------------------------------
// clear: limpia la pantalla, pone ' ' en todos los lugares disponibles
//-----------------------------------------------------------------------
// Argumentos:
// - position: La posicion de la pantalla que se desea limpiar
//-----------------------------------------------------------------------
void clear(positionType position){
    // Si borramos toda la pantalla debemos tambien reiniciar todos las porciones
    if(position == ALL){
        for(coordinatesType c = {A_ROW_START, A_COL_START, A_ROW_START, A_COL_START, A_ROW_END, A_COL_END};
            c.row_current != A_ROW_END || c.col_current != A_COL_END;
            next(&c)){
            uint8_t * aux = video_start + VIDEO_OFFSET(c);
            *(aux) = ' ';       // "Borrar" = Vacio
            *(aux + 1) = 0;     // Formato default
        }
        // Actualizamos los punteros de las distintas porciones
        coordinates[ALL].row_current = coordinates[ALL].row_start;
        coordinates[ALL].col_current = coordinates[ALL].col_start;
        coordinates[LEFT].row_current = coordinates[LEFT].row_start;
        coordinates[LEFT].col_current = coordinates[LEFT].col_start;
        coordinates[RIGHT].row_current = coordinates[RIGHT].row_start;
        coordinates[RIGHT].col_current = coordinates[RIGHT].col_start;
        return;
    }
    // Borramos todos los caracteres impresos en la porcion (LEFT o RIGHT) pasada por parametros
    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
        c.row_current != coordinates[position].row_current || c.col_current != coordinates[position].col_current;
        next(&c)){
        uint8_t * aux = video_start + VIDEO_OFFSET(c);
        *(aux) = ' ';       // "Borrar" = Vacio
        *(aux + 1) = 0;     // Formato default
    }
    // Reiniciamos la escritura en dicha porcion
    coordinates[position].row_current = coordinates[position].row_start;
    coordinates[position].col_current = coordinates[position].col_start;
}

//-----------------------------------------------------------------------
// delete_last_char: borra el ultimo caracter ingresado
//-----------------------------------------------------------------------
// Argumentos:
//  position: La posicion de la pantalla donde se desea borrar el ultimo caracter
//-----------------------------------------------------------------------
void delete_last_char(positionType position){
    // Solo debo borrar si no estoy en la posicion inicial de la porcion pasada por parametros (pues sino no habria nada para borrar)
    if(!(coordinates[position].row_current == coordinates[position].row_start &&
         coordinates[position].col_current == coordinates[position].col_start)){
        // Si el proximo caracter es al principio de una fila (distinta de la primera) entonces debo borrar el ultimo caracter
        // ingresado de la fila anterior
        if(coordinates[position].col_current == coordinates[position].col_start){
            // Me muevo a la fila anterior
            coordinates[position].row_current--;
            // Para la columna es mas dificil, porque no esta asegurado que el ultimo caracter se imprimio en la ultima posicion
            // Por lo que debo encontrar la primera posicion en la que hay un caracter distinto de ' ' (vacio) empezando por el final
            // para hallar el ultimo caracter ingresado
            // Si llego al principio de la fila, entonces puede ser que se ingreso un caracter y se presiono ENTER o directamente
            // se presiono ENTER
            coordinates[position].col_current = coordinates[position].col_end;
            while(*(video_start + 160 * coordinates[position].row_current + coordinates[position].col_current) == ' ' &&
                  coordinates[position].col_current > coordinates[position].col_start){
                coordinates[position].col_current -= NEXT_COL;
            }
        }
            // En cambio, si estoy en la mitad o al final de una fila, entonces debo borrar el caracter en la posicion anterior
        else {
            coordinates[position].col_current -= NEXT_COL;
        }
        // Borro el ultimo caracter ingresado
        *(video_start + 160 * coordinates[position].row_current + coordinates[position].col_current) = ' ';
        *(video_start + 160 * coordinates[position].row_current + coordinates[position].col_current + 1) = 0;
    }
}


//-----------------------------------------------------------------------
// uintToBase: Convierte un entero en la base indica por parametro en un string
//-----------------------------------------------------------------------
// Argumentos:
//  value: el valor del entero
//  buffer: el string sobre cual copiar
//  base: la base a convertir del entero
//-----------------------------------------------------------------------
static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    //Calculate characters for each digit
    do
    {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    }
    while (value /= base);

    // Terminate string in buffer.	(El \0 del string)
    *p = 0;

    //Reverse string in buffer. (Notar que al hacer el pasaje de int a char, se tiene el numero al reves pues se analiza de derecha a izquierda)
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

//-----------------------------------------------------------------------
// to_hex: Devuelve un entero hexadecimal en un string
//-----------------------------------------------------------------------
// Argumentos:
//  str: el string sobre cual copiar
//  val: el valor del entero
//-----------------------------------------------------------------------
void to_hex(char * str, uint64_t val){
    uintToBase(val,str,16);
}

//-----------------------------------------------------------------------
// to_dec: Devuelve un entero decimal en un string
//-----------------------------------------------------------------------
// Argumentos:
//  str: el string sobre cual copiar
//  val: el valor del entero
//-----------------------------------------------------------------------
void to_decimal(char * str, uint64_t val){
    uintToBase(val,str,10);
}

//#include <video_driver.h>
//#include <stdint.h>
//
//#define WIDTH 160
//#define HEIGHT 25
//#define NEXT_COL 2
//#define MID_WIDTH (WIDTH/2)
//
//#define L_ROW_START 0
//#define L_COL_START 0
//
//#define L_ROW_END (HEIGHT - 1)
//#define L_COL_END (MID_WIDTH - 2 * NEXT_COL)
//
//#define R_ROW_START 0
//#define R_COL_START (MID_WIDTH + 2 * NEXT_COL)
//
//#define R_ROW_END (HEIGHT - 1)
//#define R_COL_END (WIDTH - NEXT_COL)
//
//#define A_ROW_START 0
//#define A_COL_START 0
//
//#define A_ROW_END (HEIGHT - 1)
//#define A_COL_END (WIDTH - NEXT_COL)
//
//#define ASCII_DELETE 127
//#define SCREEN_ENDED(pos) ((pos).row_current == (pos).row_end && (pos).col_current == (pos).col_end)
//#define VIDEO_OFFSET(pos) (WIDTH*((pos).row_current) + (pos).col_current)
//
////TODO: sacar magic numbers
//typedef struct{
//    uint32_t row_start;             // La fila inicial
//    uint32_t col_start;             // La columna inicial
//    uint32_t row_current;           // La fila actual
//    uint32_t col_current;           // La columna actual
//    uint32_t row_end;               // La fila final
//    uint32_t col_end;               // La columna final
//} coordinatesType;
//
//void next(coordinatesType * position);
//void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position);
//
//coordinatesType coordinates[3] = {  {A_ROW_START, A_COL_START, A_ROW_START, A_COL_START, A_ROW_END, A_COL_END},
//                                    {L_ROW_START, L_COL_START, L_ROW_START, L_COL_START, L_ROW_END, L_COL_END},
//                                    {R_ROW_START, R_COL_START, R_ROW_START, R_COL_START, R_ROW_END, R_COL_END} };
//
//// Con estas variables podemos calcular la posicion donde se debe imprimir un caracter con la formula:
////                              video_start + WIDTH * row + column
//
////uint8_t para que cuando hacemos ++, vaya avanzando de a 1 byte
//static uint8_t * const video_start = (uint8_t *) 0xB8000;
//
//
////-----------------------------------------------------------------------
//// print_char: imprime un caracter en pantalla
////-----------------------------------------------------------------------
//// Argumentos:
//// -c: el caracter que se quiere imprimir
//// -letterFormat: el color de la letra, una constante de color definida en .h
//// -position: Indica en que posicion se debe imprimir el caracter
////-----------------------------------------------------------------------
//// Si no hay mas lugar en la pantalla, llama a video_scroll_up
////-----------------------------------------------------------------------
//void print_char(char c, formatType letterFormat, positionType position){
//    uint8_t * curr = video_start;                   // La primera direccion de video
//    // A esto se le suma el offset correspondiente
//    curr += VIDEO_OFFSET(coordinates[position]);
//    // Luego, imprimimos el caracter (si es \n hara un salto de linea)
//    print_aux(curr, c, letterFormat, position);
//    if(SCREEN_ENDED(coordinates[position])){
//        // Si llego al final de la pantalla, tengo que hacer scroll up
//        scroll_up(position);
//    }
////    if(c==ASCII_DELETE){
////        delete_last_char(position);
////    }
//    // Obtenemos la proxima posicion sobre la cual imprimir en la porcion correspondiente
//    if(c!='\n')
//        next(&coordinates[position]);
//}
//
//void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position){
//    if(c == '\n'){
//        new_line(position);
//    }else{
//        //Imprime el caracter en la posicion indicada
//        *curr = c;                      // Imprimo el ASCII
//        *(curr + 1) = letterFormat;     // Indico el color que va a tener
//    }
//}
//
////-----------------------------------------------------------------------
//// print: imprime un string en pantalla
////-----------------------------------------------------------------------
//// Argumentos:
//// -str: el string que se quiere imprimir
//// -letterFormat: el color de la letra, una constante de color definida en .h
//// -position: la posicion de la pantalla donde se desea imprimir
////-----------------------------------------------------------------------
//// Si no hay mas lugar en la pantalla, llama a video_scroll_up
////-----------------------------------------------------------------------
//void print(const char * str, formatType letterFormat, positionType position){
//    for(;*str!='\0';str++){
//        print_char(*str,letterFormat,position);
//    }
//}
//
////-----------------------------------------------------------------------
//// println: imprime un string en pantalla y salta a la proxima linea
////-----------------------------------------------------------------------
//// Argumentos:
//// -str: el caracter que se quiere imprimir
//// -letterFormat: el color de la letra, una constante de color definida en .h
//// -position: posicion de la pantalla donde se quiere imprimir
////-----------------------------------------------------------------------
//// Si no hay mas lugar en la pantalla, llama a scroll_up
////-----------------------------------------------------------------------
//void println(const char * str, formatType letterFormat, positionType position){
//    print(str,letterFormat,position);
//    new_line(position);
//}
//
////-----------------------------------------------------------------------
//// new_line: ubica al cursor en la proxima linea
////-----------------------------------------------------------------------
//// Argumentos:
//// -position: La posicion de la pantalla donde se desea ir a la proxima linea
////-----------------------------------------------------------------------
//void new_line(positionType position){
//    if(coordinates[position].row_current==coordinates[position].row_end){
//        scroll_up(position);
//    }
//    // Posicionamos la fila de la porcion correspondiente en la proxima
//    coordinates[position].row_current += 1;
//    // Posicionamos la columna de la porcion correspondiente al principio
//    coordinates[position].col_current = coordinates[position].col_start;
//}
//
////-----------------------------------------------------------------------
//// Funcion auxiliar para obtener la siguiente posicion donde tiene que ir el offset según la porcion indicado por parametro
////-----------------------------------------------------------------------
//
//void next(coordinatesType * position){
//    // Si se llego al final de una fila, paso a la proxima fila y al principio de ella
//    if(position->col_current == position->col_end){
//        position->col_current = position->col_start;
//        position->row_current++;
//    }
//    // Si no llegue al final, paso a la siguiente columna en la fila
//    else {
//        position->col_current += NEXT_COL;
//    }
//}
//
////-----------------------------------------------------------------------
//// scroll_up: Mueve los caracteres una linea arriba (la primera se pierde)
////-----------------------------------------------------------------------
//// Argumentos:
//// - position: la posicion de la pantalla donde se desea imprimir
////-----------------------------------------------------------------------
//void scroll_up(positionType position){
//    // Copiamos los caracteres de la fila n en n-1, n > 1
//    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
//        c.row_current != coordinates[position].row_current || c.col_current != coordinates[position].col_current;
//        next(&c)){
//            *(video_start + c.row_current * WIDTH + c.col_current) = *(video_start + (c.row_current+1) * WIDTH + c.col_current);
//            *(video_start + c.row_current * WIDTH + c.col_current + 1) = *(video_start + (c.row_current+1) * WIDTH + c.col_current + 1);
//    }
//    // Nos paramos en la fila anterior pues todo se subio una fila arriba
//    if(coordinates[position].row_current >= 1){
//        // Si habia llegado a la ultima fila, debo borrar los caracteres en esa fila
//        if(coordinates[position].row_current == coordinates[position].row_end){
//            for(coordinatesType c = {coordinates[position].row_end, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
//                c.row_current <= coordinates[position].row_end && c.col_current <= coordinates[position].col_end;
//                next(&c)){
//                    *(video_start + c.row_current * WIDTH + c.col_current) = ' ';
//                    *(video_start + c.row_current * WIDTH + c.col_current + 1) = 0;
//            }
//        }
//        coordinates[position].row_current--;
//    }
//    // En cambio, si estoy en la primera fila, se borro todo, por lo que se debe empezar desde el principio
//    else {
//        coordinates[position].col_current = coordinates[position].col_start;
//    }
//}
//
//// ----------------------------------------------------------------------------
//// print_lines: Imprime las lineas de separacion para cuando el usuario haga la función |
//// ----------------------------------------------------------------------------
//// Argumentos:
////  null
//// ----------------------------------------------------------------------------
//// Retorno:
////  null
//// ----------------------------------------------------------------------------
//void print_lines(){
//    uint32_t col1 = 80;                         // La posicion 78 y 80 son la mitad de la pantalla en el arreglo del driver
//    uint32_t col2 = 78;
//    for(uint32_t i = 0; i <= A_ROW_END; i++){
//        *(video_start + WIDTH * i + col1) = '|';
//        *(video_start + WIDTH * i + col1 + 1) = WHITE;
//        *(video_start + WIDTH * i + col2) = '|';
//        *(video_start + WIDTH * i + col1 + 1) = WHITE;
//    }
//}
//
////-----------------------------------------------------------------------
//// clear: limpia la pantalla, pone ' ' en todos los lugares disponibles
////-----------------------------------------------------------------------
//// Argumentos:
//// - position: La posicion de la pantalla que se desea limpiar
////-----------------------------------------------------------------------
//void clear(positionType position){
//    // Si borramos toda la pantalla debemos tambien reiniciar todos las porciones
//    if(position == ALL){
//        for(coordinatesType c = {A_ROW_START, A_COL_START, A_ROW_START, A_COL_START, A_ROW_END, A_COL_END};
//            c.row_current != A_ROW_END || c.col_current != A_COL_END;
//            next(&c)){
//                uint8_t * aux = video_start + VIDEO_OFFSET(c);
//                *(aux) = ' ';       // "Borrar" = Vacio
//                *(aux + 1) = 0;     // Formato default
//        }
//        // Actualizamos los punteros de las distintas porciones
//        coordinates[ALL].row_current = coordinates[ALL].row_start;
//        coordinates[ALL].col_current = coordinates[ALL].col_start;
//        coordinates[LEFT].row_current = coordinates[LEFT].row_start;
//        coordinates[LEFT].col_current = coordinates[LEFT].col_start;
//        coordinates[RIGHT].row_current = coordinates[RIGHT].row_start;
//        coordinates[RIGHT].col_current = coordinates[RIGHT].col_start;
//        return;
//    }
//    // Borramos todos los caracteres impresos en la porcion (LEFT o RIGHT) pasada por parametros
//    for(coordinatesType c = {coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_start, coordinates[position].col_start, coordinates[position].row_end, coordinates[position].col_end};
//        c.row_current != coordinates[position].row_current || c.col_current != coordinates[position].col_current;
//        next(&c)){
//            uint8_t * aux = video_start + VIDEO_OFFSET(c);
//            *(aux) = ' ';       // "Borrar" = Vacio
//            *(aux + 1) = 0;     // Formato default
//    }
//    // Reiniciamos la escritura en dicha porcion
//    coordinates[position].row_current = coordinates[position].row_start;
//    coordinates[position].col_current = coordinates[position].col_start;
//}
//
////-----------------------------------------------------------------------
//// delete_last_char: borra el ultimo caracter ingresado
////-----------------------------------------------------------------------
//// Argumentos:
////  position: La posicion de la pantalla donde se desea borrar el ultimo caracter
////-----------------------------------------------------------------------
//void delete_last_char(positionType position){
//    // Solo debo borrar si no estoy en la posicion inicial de la porcion pasada por parametros (pues sino no habria nada para borrar)
//    if(!(coordinates[position].row_current == coordinates[position].row_start &&
//        coordinates[position].col_current == coordinates[position].col_start)){
//            // Si el proximo caracter es al principio de una fila (distinta de la primera) entonces debo borrar el ultimo caracter
//            // ingresado de la fila anterior
//            if(coordinates[position].col_current == coordinates[position].col_start){
//                // Me muevo a la fila anterior
//                coordinates[position].row_current--;
//                // Para la columna es mas dificil, porque no esta asegurado que el ultimo caracter se imprimio en la ultima posicion
//                // Por lo que debo encontrar la primera posicion en la que hay un caracter distinto de ' ' (vacio) empezando por el final
//                // para hallar el ultimo caracter ingresado
//                // Si llego al principio de la fila, entonces puede ser que se ingreso un caracter y se presiono ENTER o directamente
//                // se presiono ENTER
//                coordinates[position].col_current = coordinates[position].col_end;
//                while(*(video_start + 160 * coordinates[position].row_current + coordinates[position].col_current) == ' ' &&
//                    coordinates[position].col_current > coordinates[position].col_start){
//                        coordinates[position].col_current -= NEXT_COL;
//                }
//            }
//            // En cambio, si estoy en la mitad o al final de una fila, entonces debo borrar el caracter en la posicion anterior
//            else {
//                coordinates[position].col_current -= NEXT_COL;
//            }
//            // Borro el ultimo caracter ingresado
//            *(video_start + 160 * coordinates[position].row_current + coordinates[position].col_current) = ' ';
//            *(video_start + 160 * coordinates[position].row_current + coordinates[position].col_current + 1) = 0;
//    }
//}
//
//
////-----------------------------------------------------------------------
//// uintToBase: Convierte un entero en la base indica por parametro en un string
////-----------------------------------------------------------------------
//// Argumentos:
////  value: el valor del entero
////  buffer: el string sobre cual copiar
////  base: la base a convertir del entero
////-----------------------------------------------------------------------
//static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
//{
//    char *p = buffer;
//    char *p1, *p2;
//    uint32_t digits = 0;
//
//    //Calculate characters for each digit
//    do
//    {
//        uint32_t remainder = value % base;
//        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
//        digits++;
//    }
//    while (value /= base);
//
//    // Terminate string in buffer.	(El \0 del string)
//    *p = 0;
//
//    //Reverse string in buffer. (Notar que al hacer el pasaje de int a char, se tiene el numero al reves pues se analiza de derecha a izquierda)
//    p1 = buffer;
//    p2 = p - 1;
//    while (p1 < p2)
//    {
//        char tmp = *p1;
//        *p1 = *p2;
//        *p2 = tmp;
//        p1++;
//        p2--;
//    }
//
//    return digits;
//}
//
////-----------------------------------------------------------------------
//// to_hex: Devuelve un entero hexadecimal en un string
////-----------------------------------------------------------------------
//// Argumentos:
////  str: el string sobre cual copiar
////  val: el valor del entero
////-----------------------------------------------------------------------
//void to_hex(char * str, uint64_t val){
//    uintToBase(val,str,16);
//}
//
////-----------------------------------------------------------------------
//// to_dec: Devuelve un entero decimal en un string
////-----------------------------------------------------------------------
//// Argumentos:
////  str: el string sobre cual copiar
////  val: el valor del entero
////-----------------------------------------------------------------------
//void to_decimal(char * str, uint64_t val){
//    uintToBase(val,str,10);
//}