#include <syscalls.h>
#include <video_driver.h>

int read_handler(char * str){
    extern char buffer[];           // Obtenemos el buffer y los punteros de keyboard
    extern int read;
    extern int write;

    if (read == write)
        return 0;                   // No hay para leer (el puntero de escritura y lectura estan en la misma posicion), por lo que se devuelve 0
    *str = buffer[read];            // Si hay para leer, guardamos el siguiente caracter en la primera posicion del string
    str[1] = '\0';
    read = (read == BUFF_LENGTH-1) ? 0 : read+1;    // Movemos el puntero de lectura a la siguiente posicion
    return 1;                       // Devolvemos la cantidad de caracteres leidos
}

int write_handler(char * str, int format){
    positionType position = LEFT;
    print(char * str, int format, positionType position);        // Imprime por pantalla
}


int exec_handler(void * program){
    return 0;
    // addProcess(program);
}

int exit_handler(int * errCode){
    return 0;
    // finishProcess();
}
