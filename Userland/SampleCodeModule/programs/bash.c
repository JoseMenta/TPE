#include <bash.h>

//char buffer[100];
//int buffer_index = 0;
//
//void analyze_buffer(void);
//void clean_buffer(void);
//
////---------------------------------------------------------------------------------
//// bash: Programa a correr al inciar el UserSpace
////---------------------------------------------------------------------------------
//// Argumentos:
////   void
////---------------------------------------------------------------------------------
//// Retorno
////   void
////---------------------------------------------------------------------------------
//void bash(void){
//    print_string("$ ", WHITE);
//    while(1){
//        int c = get_char();
//        if(c =='\n') {
//            analyze_buffer();
//            clean_buffer();
//        }
//        if(c!=-1){
//            buffer[buffer_index++] = c;
//            buffer[buffer_index] = '\0';
//        }
//    }
//}
//
////---------------------------------------------------------------------------------
//// analyze_buffer: Analiza los caracteres ingresados desde el ultimo ENTER
////---------------------------------------------------------------------------------
//// Argumentos:
////   void
////---------------------------------------------------------------------------------
//// Retorno
////   void
////---------------------------------------------------------------------------------
//// Ejecuta uno o dos procesos nuevos si el string en el buffer es valido
////---------------------------------------------------------------------------------
//void analyze_buffer(void) {
//    // Obtenemos los strings sin espacios de los ingresado en la terminal
//    char * tokens = strtok(buffer, " ");
//    // Si no se ingreso texto, solo ENTER, no se hace nada
//    if(tokens == NULL){
//        return;
//    }
//    // Si se escribe "logout", bash se cierra y se apaga la compu
//    if(strcmp(tokens, LOGOUT) == 0){
//        tokens = strtok(NULL, tokens);
//        // Verificamos que solo se halla ingresado "logout" y nada mas
//        if(tokens == NULL){
//            sys_exit(0);
//        }
//        else {
//            print_string("error: expresion invalida", RED);
//            return;
//        }
//    }
//
//    // El primer string debe ser un programa valido
//    void* program_a =  get_program(tokens);
//    // Si no se encontro programa, entonces no es un string valido
//    if(program_a == NULL){
//        // Lanzar error: print rojo
//        print_string("error: programa invalido", RED);
//        return;
//    }
//    // Para iterar sobre los tokens, se debe hacer strtok(NULL, tokens), donde NULL es una constante igual a 0
//    tokens = strtok(0, tokens);
//    // Si solo habia un string (y valido) entonces se ejecuta un programa en toda la pantalla
//    if (tokens == NULL) {
//        uint64_t program[1] = {(uint64_t)program_a};
//        sys_exec(1, (void *)program);
//        return;
//    }
//    // En cambio, si hay mas de un string, el proximo debe ser un pipe
//    // Si no lo es, entonces es una expresion invalida
//    if(strcmp(tokens, "|") != 0) {
//        // Lanzar error: print rojo
//        print_string("error: no usa | como separador de programas", RED);
//        return;
//    }
//    tokens = strtok(0, tokens);
//    // Si no hay un tercer string tambien es invalido pues debe ser 'pgm1 | pgm2'
//    if(tokens == NULL){
//        // Lanzar error: print rojo
//        print_string("error: programa de consola derecha ausente", RED);
//        return;
//    }
//    // Luego, el tercer string debe ser un programa valido
//    void* program_b = get_program(tokens);
//    // Si no lo es lanza error
//    if (program_b == NULL) {
//        // Lanzar error: print rojo
//        print_string("error: programa para consola derecha invalido", RED);
//        return;
//    }
//    // Y no debe haber mas strings, para asi ejecutar dos programas
//    tokens = strtok(0, tokens);
//    if (tokens == NULL) {
//        void* program[2] = {program_a, program_b};
//        sys_exec(2, (void *)program);
//        return;
//    }
//    // Si los hay lanza error
//    else{
//        // Lanzar error: print rojo
//        print_string("error: cantidad de programas invalida", RED);
//        return;
//    }
//}
//
////---------------------------------------------------------------------------------
//// clean_buffer: Limpia el buffer
////---------------------------------------------------------------------------------
//// Argumentos:
////   void
////---------------------------------------------------------------------------------
//// Retorno
////   void
////---------------------------------------------------------------------------------
//void clean_buffer(void){
//    for(int i = 0; i < buffer_index; i++){
//        buffer[i] = '\0';
//    }
//    buffer_index = 0;
//}