#include <keyboard.h>
#include <scheduler.h>
#include <interrupts.h>
#include <queue.h>

#define IS_ALPHA(x) ((x) >= 'a' && (x) <= 'z') ? 1 : 0
#define IS_REFERENCEABLE(x) ((x) <= KEYBOARD_REFERENCE_LENGTH && keyboard_reference[(x)]!='\0')


//El buffer guarda el ASCII del valor presionado en el teclado
queue_t queue = {{0}};
static int key_case = -1;               // Estado actual del formato de la letra (mayuscula: 1, o minuscula: -1)
static int key_case_default = -1;       // Estado default del formato de la letra

static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int left_state = 1;
static int right_state = 1;
static int all_state = 1;
uint8_t regs_saved = 0;

uint64_t* get_inforeg_context();
//teclado Mac
static int keyboard_reference[] = {'\0','\0','1','2','3','4','5',
                                   '6','7','8','9','0','-','=',
                                   ASCII_DELETE,'\t','q','w','e','r','t',
                                   'y','u','i','o','p','[',']','\n',
                                   '\0','a','s','d','f','g','h','j',
                                   'k','l',';','\'','|','\0','\\',
                                   'z','x','c','v','b','n','m',',',
                                   '.','/','\0', '\0', ALT, ' '};
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
    // Obtenemos la tecla ingresada
    uint8_t key = get_keyboard_scan_code();

    //Agrego la logica para cuando quiere volver
    extern uint8_t want_to_return;
    if(key == ESC){
        //Si pone ESC, se cortan todos los procesos que se encuentren corriendo en el momento y vuelve al proceso que los llamo
        want_to_return = 1;
        change_context();
        // Al reiniciar un proceso antiguo, se pierde el estado de los procesos pues se eliminaron
        all_state = left_state = right_state = 1;
    }

    // Logica para escribir en mayuscula
    else if(key == SHIFT1 || key == SHIFT2) {
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

    // Logica para el pausado y reanudado de procesos
    else if(key == CTRL){
        ctrl_pressed = 1;
    }
    else if (key == CTRL+RELEASED) {
        ctrl_pressed = 0;
    }

    // Logica para detener procesos
    else if(key == ALT){
        alt_pressed = 1;
    } else if (key == ALT + RELEASED){
        alt_pressed = 0;
    }

    // Guardamos en el buffer solo aquellas teclas que puedan ser referenciables: letras, digitos y espacios (\n, \t, etc)
    else if(IS_REFERENCEABLE(key)){
        // Suspension y reanudacion de procesos
        // Primero debemos verificar si en vez de agregar una letra al buffer, se desea suspende o reanudar un proceso
        if(keyboard_reference[key] == 'f' && ctrl_pressed){
            //all_state: 1 si sigue corriendo, 0 si no
            if(all_state){
                // Suspende el proceso si esta corriendo en pantalla completo y cambia el flag de all_state al asignarse
                // Si no esta corriendo en pantalla completa (se hizo un split), entonces no hay que cambiar el flag
                all_state = suspend_full();
            }else{
                // Siempre podra restartear el proceso si all_state = 0 pues tuvo que poder suspenderlo
                all_state = restart_full();
            }
        }
        else if(keyboard_reference[key] == 'l' && ctrl_pressed){
            //left_state: 1 si sigue corriendo, 0 si no
            if(left_state){
                // Suspende el proceso si esta corriendo en pantalla split y cambia el flag de left_state al asignarse
                // Si no esta corriendo en split (esta en pantalla completa), entonces no hay que cambiar el flag
                left_state = suspend_left();
            }else{
                // Siempre podra restartear el proceso si left_state = 0 pues tuvo que poder suspenderlo
                left_state = restart_left();
            }
        }
        else if(keyboard_reference[key] == 'r' && ctrl_pressed){
            //right_state: 1 si sigue corriendo, 0 si no

            if(right_state){
                // Suspende el proceso si esta corriendo en pantalla split y cambia el flag de right_state al asignarse
                // Si no esta corriendo en split (esta en pantalla completa), entonces no hay que cambiar el flag
                right_state = suspend_right();
            }else{
                // Siempre podra restartear el proceso si right_state = 0 pues tuvo que poder suspenderlo
                right_state = restart_right();
            }
        }


        // Detenimiento de procesos
        else if(keyboard_reference[key] == 'f' && alt_pressed){
            all_state = kill_full();
        }
        else if (keyboard_reference[key] == 'l' && alt_pressed){
            left_state = kill_left();
        }
        else if(keyboard_reference[key] == 'r' && alt_pressed){
            right_state = kill_right();
        }
        else if(keyboard_reference[key] == 's' && ctrl_pressed){
            copy_curr_context_to_inforeg_context();
        }


        else if (key_case > 0 && IS_ALPHA(keyboard_reference[key])){
            // Es una mayuscula
            //buffer[write] = keyboard_reference[key] - UPPER_OFFSET;
            enqueue(&queue,keyboard_reference[key] - UPPER_OFFSET);
        } else {
            // Es una minuscula u otro tipo de caracter que no es letra
            //buffer[write] = keyboard_reference[key];
            enqueue(&queue,keyboard_reference[key]);
        }
    }
}

void copy_curr_context_to_inforeg_context(){
    regs_saved = 1;
    uint64_t* curr_context = getCurrContext(); //Guardo el contexto en la interrupcion
    uint64_t* inforeg_context = get_inforeg_context();
    for(int i = 0; i<REGISTERS_COUNT; i++){
        inforeg_context[i] = curr_context[i];//Copio el contexto para cuando lo llamen
    }
    return;
}
void clear_keyboard_buffer(){
    clear_queue(&queue);
}

