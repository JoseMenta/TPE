#include <keyboard.h>
#include <queue.h>

//Constantes para los scan_code
#define BLOCK_MAYUSC 58
#define SHIFT1 42
#define SHIFT2 54
#define RELEASED 128
#define UPPER_OFFSET ('a'-'A')  //0x20
#define KEYBOARD_REFERENCE_LENGTH 54
#define IS_ALPHA(x) ((x) >= 'a' && (x) <= 'z') ? 1 : 0
#define IS_REPRESENTABLE(x) ((x) <= KEYBOARD_REFERENCE_LENGTH && keyboard_reference[(x)]!='\0')


//El buffer guarda el ASCII del valor presionado en el teclado
queue_t queue = {{0}};
//char buffer[BUFF_LENGTH];
static int key_case = -1;               // Estado actual del formato de la letra (mayuscula: 1, o minuscula: -1)
static int key_case_default = -1;       // Estado default del formato de la letra
//uint8_t write = 0;                    // Puntero de escritura
//uint8_t read = 0;                     // Puntero de lectura

//teclado Mac
static int keyboard_reference[] = {'\0','\0','1','2','3','4','5',
                                   '6','7','8','9','0','-','=',
                                   '\0',' ','q','w','e','r','t',
                                   'y','u','i','o','p','[',']','\\',
                                   '\0','a','s','d','f','g','h','j',
                                   'k','l',';','\'','\n','\0','\\',
                                   'z','x','c','v','b','n','m',',',
                                   '.','/','\0'};
//teclado windows
/*static int keyboard_reference2[] = {'0', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
                                    '\'', '¡', '0', '0', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
                                    'o', 'p', '\'', '+', '\n', '0', 'a', 's', 'd', 'f', 'g', 'h',
                                    'j', 'k', 'l', 'ñ', '{', '0', '^', '0', 'z', 'x', 'c', 'v',
                                    'b', 'n', 'm', ',', '.', '-', '^', '0', '0', '\t'}
*/

// Si se presiona una tecla, esta se almacena en la siguiente posicion en el buffer
//TODO: agregar manejo de combinacion de teclas para suspender/reiniciar procesos
void keyboard_handler(){

    uint8_t key = get_keyboard_scan_code();

    if(key == SHIFT1 || key == SHIFT2) {
        //Estamos usando que cuando se mantiene shift, no se mandan varias interrupciones
        //Si ese fuese el caso, entonces el comportamiento seria aleatorio
        key_case = key_case_default * -1;   //Invierte el default
    }
    else if (key == SHIFT1+RELEASED || key == SHIFT2+RELEASED){

        key_case = key_case_default;        // Vuelve al estado default
    }
    //Tira muchas teclas, es raro como funciona
    //Si arranco con la tecla activada, me aparece como que estoy en minusculas igual
    //Ademas, dependiendo de como lo toque, hace como 1 o 3 interrupciones
    //Es raro, shift ya funciona
//    else if (key == BLOCK_MAYUSC){
//
//        ncPrint("BLOCK");
//        // Cambia el default
//        key_case_default *= -1;
//        key_case = key_case_default;
//    }
    // TODO: CAMBIAR LA LOGICA (NO SOLO REPRESENTABLES)
    // Solo guardamos los caracteres que tienen una representacion grafica
    else if(IS_REPRESENTABLE(key)){
        //Tenemos que guardar esa tecla
        if (key_case > 0 && IS_ALPHA(keyboard_reference[key])){
            // Es una mayuscula
            //buffer[write] = keyboard_reference[key] - UPPER_OFFSET;
            enqueue(&queue,keyboard_reference[key] - UPPER_OFFSET);
        } else {
            // Es una minuscula u otro tipo de caracter que no es letra
            //buffer[write] = keyboard_reference[key];
            enqueue(&queue,keyboard_reference[key]);
        }
//        write = (write == BUFF_LENGTH-1) ? 0 : write+1;
    }

}
