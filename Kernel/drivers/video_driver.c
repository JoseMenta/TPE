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

#define SCREEN_ENDED(pos,row_end,col_end) (pos.row == row_end && pos.col == col_end)
#define OFFSET(pos) (WIDTH*((pos).row) + (pos).col)

typedef struct{
    uint32_t row;
    uint32_t col;
} coordinatesType;

//left_next: es la posicion siguiente al ultimo caracter copiado en la pantalla izquierda
void left_next(coordinatesType * position);
//right_next: es la posicion siguenete al ultimo caracter copiado en la pantalla derecha
void right_next(coordinatesType * position);
//all_next: es la posicion siguiente al ultimo caracter copaido en toda la pantalla
void all_next(coordinatesType * position);

void print_aux(uint8_t * curr, char c, formatType letterFormat, positionType position);

//Variables para la pantalla izquierda (puntero para fila y para columna)
coordinatesType left = {L_ROW_START,L_COL_START};

//Variables para la pantalla derecha
coordinatesType right = {R_ROW_START, R_COL_START};

//Variables para toda la pantalla
coordinatesType all = {A_ROW_START, A_COL_START};

// Con estas variables podemos calcular la posicion donde se debe imprimir un caracter con la formula:
//                              video_start + WIDTH * row + column

//uint8_t para que cuando hacemos ++, vaya avanzando de a 1 byte
static uint8_t * const video_start = (uint8_t *) 0xB8000;


//-----------------------------------------------------------------------
// print_char: imprime un caracter en pantalla
//-----------------------------------------------------------------------
// Argumentos:
// -c: el caracter que se quiere imprimir
// -leterFormat: el color de la letra, una constante de color definida en .h
// -position: Indica en que posicion se debe imprimir el caracter
//-----------------------------------------------------------------------
// Si no hay mas lugar en la pantalla, llama a video_scroll_up
//-----------------------------------------------------------------------
void print_char(char c, formatType letterFormat, positionType position){
    uint8_t * curr = video_start;                   // La primera direccion de video
    // A esto se le suma el offset correspondiente
    if(position==LEFT){
        curr += OFFSET(left);
        print_aux(curr,c,letterFormat,position);
        if(SCREEN_ENDED(left,L_ROW_END,L_COL_END)){
            // Tengo que hacer scroll up
            scroll_up(position);
        }
        left_next(&left);

    } else if (position==RIGHT){
        curr += OFFSET(right);
        print_aux(curr,c,letterFormat, position);
        if(SCREEN_ENDED(right,R_ROW_END, R_COL_END)){ // Llegue al final de la pantalla
            //tengo que hacer scroll up
            scroll_up(position);
        }
        right_next(&right);

    }else{
        // position == ALL
        curr += OFFSET(all);
        print_aux(curr,c,letterFormat,position);
        if(SCREEN_ENDED(all, A_ROW_END, A_COL_END)){
            //Tengo que hacer scroll up
            scroll_up(position);
        }
        all_next(&all);
    }
    // Luego, dependiendo de si el caracter es \n o no, realizamos un salto de linea o imprimimos el caracter, respectivamente
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
void print(char * str, formatType letterFormat, positionType position){
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
void println(char * str, formatType letterFormat, positionType position){
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
        left.row += 1;                  // Posicionamos la fila izquierda en la proxima fila
        left.col = L_COL_START;         // Posicionamos la columna al principio
    }
    else if(position == RIGHT){
        right.row += 1;                 // Posicionamos la fila derecha en la proxima fila
        right.col = R_COL_START;        // Posicionamos la columna al principio (mitad + 1)
    }
    else{
        //position == ALL
        all.row += 1;                   // Posicionamos la fila derecha en la proxima fila
        all.col = A_COL_START;          // Posicionamos la columna al principio
    }
}

//-----------------------------------------------------------------------
// Funciones auxiliares para obtener la siguiente posicion donde tiene que ir el offset dependiendo del caso
//-----------------------------------------------------------------------
void left_next(coordinatesType * position){
    if(position->col == L_COL_END){                 // Si se llego al final de una fila, paso a la proxima fila y al principio de ella
        position->col = L_COL_START;
        position->row++;
    } else {                                        // Si no llegue al final, paso a la siguiente columna en la fila
        position->col += NEXT_COL;
    }
}

void right_next(coordinatesType * position){
    if(position->col == R_COL_END){
        position->col = R_COL_START;
        position->row++;
    } else {
        position->col += NEXT_COL;
    }
}

void all_next(coordinatesType * position){
    if(position->col == A_COL_END){
        position->col = A_COL_START;
        position->row++;
    } else {
        position->col += NEXT_COL;
    }
}

//-----------------------------------------------------------------------
// scroll_up: Mueve los caracteres una linea arriba (la primera se pierde)
//-----------------------------------------------------------------------
// Argumentos:
// -position: la posicion de la pantalla donde se desea imprimir
//-----------------------------------------------------------------------
void scroll_up(positionType position){
    if(position == LEFT){
        //Voy hasta L_ROW_END-2 porque en el ciclo hago un +1 para copiar la fila de abajo
        for(coordinatesType l = {L_ROW_START, L_COL_START}; l.row != left.row || l.col != left.col; left_next(&l)){
            //copio toda la linea de abajo en la de arriba
            *(video_start + l.row*WIDTH + l.col ) = *(video_start +(l.row+1)*WIDTH + l.col );
        }
        if(left.row >=1){
            //Solo hay 1 caso donde tengo que llenar otra vez con 0's y es si el cursor esta en la ultima linea
            //Entonces por las dudas lleno a la ultima linea con 0's
            if(left.row == L_ROW_END) {
                for (coordinatesType l = {L_ROW_END , L_COL_START}; l.row <= L_ROW_END && l.col <= L_COL_END; left_next(&l)) {
                    //copio toda la linea de abajo en la de arriba
                    *(video_start + l.row * WIDTH + l.col) = ' '; //lleno con espacios
                    *(video_start + l.row * WIDTH + l.col + 1) = 0; //de color negro el fondo
                }
            }
            left.row = left.row - 1; //voy al principio de la linea anterior
        }else{
            //ya estoy en la primera fila
            //me tengo que mover a la primera columna
            left.col = L_COL_START;
        }
    }
    else if (position == RIGHT){
        for(coordinatesType r = {R_ROW_START,R_COL_START}; r.row != right.row || r.col != right.col ; right_next(&r)){
            //copio toda la linea de abajo en la de arriba
            *(video_start + r.row*WIDTH + r.col ) = *(video_start +(r.row+1)*WIDTH + r.col);
        }
        if(right.row >=1){
            //si ya estoy en la segunda fila o mas abajo, tengo que llenar donde estoy con 0's
            if(right.row==R_ROW_END) {
                for (coordinatesType r = {R_ROW_END, R_COL_START};
                     r.row <= R_ROW_END && r.col <= R_COL_END; right_next(
                        &r)) { //me ubico en el principio de la pantalla derecha
                    *(video_start + r.row * WIDTH + r.col) = ' '; //lleno con espacios
                    *(video_start + r.row * WIDTH + r.col + 1) = 0; //de color negro el fondo
                }
            }
            right.row = right.row - 1; //voy al principio de la linea anterior
        }else{
            right.col = R_COL_START;
        }
    }
    else {
        //position == all
        for(coordinatesType a = {A_ROW_START,A_ROW_START}; a.row != all.row || a.col != all.col ; all_next(&a)){
            //copio toda la linea de abajo
            *(video_start + a.row*WIDTH + a.col ) = *(video_start +(a.row+1)*WIDTH + a.col);
        }
        if(all.row>=1){
            //si ya estoy en la segunda fila o mas abajo, tengo que llenar donde estoy con 0's
            if(all.row==A_ROW_END)
            for(coordinatesType a = {A_ROW_END,A_COL_START}; a.row <= A_ROW_END && a.col <= A_COL_END; all_next(&a)){
                *(video_start + a.row * WIDTH + a.col) = ' '; //lleno con espacios
                *(video_start + a.row * WIDTH + a.col + 1) = 0; //de color negro el fondo
            }
            all.row = right.row - 1; //voy al principio de la linea anterior

        }else{
            all.col = L_COL_START;
        }
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
    uint32_t col1 = 80;
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
    // Si quiero borrar la parte izquierda, se debe borrar todos los caracteres impresos en el lado izquierdo
    if(position == LEFT){
        for(coordinatesType l = {L_ROW_START, L_COL_START}; l.row != left.row || l.col != left.col; left_next(&l)){
            uint8_t * aux = video_start + WIDTH * l.row + l.col;
            *(aux) = ' ';       // "Borrar" = Vacio
            *(aux + 1) = 0;     // Formato default
        }
        left = (coordinatesType){L_ROW_START, L_COL_START}; // Reinicio la estructura de escritura para el lado izquierdo
    }
    // Si quiero borrar la parte derecha, se debe borrar todos los caracteres impresos en el lado derecho
    else if(position == RIGHT){
        for(coordinatesType r = {R_ROW_START, R_COL_START}; r.row != right.row || r.col != right.col; right_next(&r)){
            uint8_t * aux = video_start + WIDTH * r.row + r.col;
            *(aux) = ' ';       // "Borrar" = Vacio
            *(aux + 1) = 0;     // Formato default
        }
        right = (coordinatesType){R_ROW_START, R_COL_START}; // Reinicio la estructura de escritura para el lado derecho
    }
    // Si quiero borrar la pantalla, se debe borrar todos los caracteres impresos en la pantalla
    else{
        // position == ALL
        for(coordinatesType a = {A_ROW_START, A_COL_START}; a.row != all.row || a.col != a.col; all_next(&a)){
            uint8_t * aux = video_start + WIDTH * a.row + a.col;
            *(aux) = ' ';       // "Borrar" = Vacio
            *(aux + 1) = 0;     // Formato default
        }
        left = (coordinatesType){L_ROW_START, L_COL_START};     // Reinicio la estructura de escritura para el lado izquierdo
        right = (coordinatesType){R_ROW_START, R_COL_START};    // Reinicio la estructura de escritura para el lado derecho
        all = (coordinatesType){A_ROW_START, A_COL_START};      // Reinicio la estructura de escritura para la pantalla
    }
}

//-----------------------------------------------------------------------
// delete_last_char: borra el ultimo caracter ingresado
//-----------------------------------------------------------------------
// Argumentos:
//  position: La posicion de la pantalla donde se desea borrar el ultimo caracter
//-----------------------------------------------------------------------
void delete_last_char(positionType position){
    uint8_t * to_delete;
    if(position == LEFT){
        if(!(left.row == L_ROW_START && left.col == L_COL_START)){
            if(left.col == L_COL_START){
                left.row--;
                left.col = L_COL_END;
                while(*(video_start + 160 * left.row + left.col) == ' ' && left.col > L_COL_START){
                    left.col -= NEXT_COL;
                }
            }
            else {
                left.col -= NEXT_COL;
            }
            *(video_start + 160 * left.row + left.col) = ' ';
            *(video_start + 160 * left.row + left.col + 1) = 0;
        }
    }
    else if (position == RIGHT){
        if(!(right.row == R_ROW_START && right.col == R_COL_START)){
            if(right.col == R_COL_START){
                right.row--;
                right.col = R_COL_END;
                while(*(video_start + 160 * right.row + right.col) == ' ' && right.col > R_COL_START){
                    right.col -= NEXT_COL;
                }
            }
            else {
                right.col -= NEXT_COL;
            }
            *(video_start + 160 * right.row + right.col) = ' ';
            *(video_start + 160 * right.row + right.col + 1) = 0;
        }
    }
    else {
        // position == ALL
        if(!(all.row == A_ROW_START && all.col == A_COL_START)){
            if(all.col == A_COL_START){
                all.row--;
                all.col = A_COL_END;
                while(*(video_start + 160 * all.row + all.col) == ' ' && all.col > A_COL_START){
                    all.col -= NEXT_COL;
                }
            }
            else {
                all.col -= NEXT_COL;
            }
            *(video_start + 160 * all.row + all.col) = ' ';
            *(video_start + 160 * all.row + all.col + 1) = 0;
        }
    }
}

