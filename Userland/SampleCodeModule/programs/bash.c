//#include <bash.h>
//#include <libc.h>
//
//char buffer[100];
//int buffer_index = 0;
//
//int analyze_buffer(void);
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
//        char c = get_char();
//        if(c =='\n') {
//            //analyze buffer hace el manejo de sys_exc.
//            //si corre un programa limpia la pantalla realiza la sys_exec() y vuelve a limpairla
//            //sino entonces dejo el \n e imprimo nueva linea con el buffer en 0
//            if(!analyze_buffer()){
//                print_string("\n$ ", WHITE);
//            }
//            clean_buffer();
//        }else if(c==ASCII_DELETE){
//            //si quiero borrar, al imprimirlo ya lo saque del video driver
//            //ahora lo saco del buffer.
//            buffer[--buffer_index] = '\0';
//            print_string("\n$ ", WHITE);
//        }else if(c!=-1){
//            buffer[buffer_index++] = c;
//            buffer[buffer_index] = '\0';
//            print_string("\n$ ", WHITE);
//        }
//    }
//}
//
//
////
//////---------------------------------------------------------------------------------
////// analyze_buffer: Analiza los caracteres ingresados desde el ultimo ENTER
//////---------------------------------------------------------------------------------
////// Argumentos:
//////   void
//////---------------------------------------------------------------------------------
////// Retorno
//////   void
//////---------------------------------------------------------------------------------
////// Ejecuta uno o dos procesos nuevos si el string en el buffer es valido
//////---------------------------------------------------------------------------------
////void analyze_buffer(void) {
////    // Obtenemos los strings sin espacios de los ingresado en la terminal
////    char * tokens = strtok(buffer, " ");
////    // Si no se ingreso texto, solo ENTER, no se hace nada
////    if(tokens == NULL){
////        return;
////    }
////    // Si se escribe "logout", bash se cierra y se apaga la compu
////    if(strcmp(tokens, LOGOUT) == 0){
////        tokens = strtok(NULL, tokens);
////        // Verificamos que solo se halla ingresado "logout" y nada mas
////        if(tokens == NULL){
////            sys_exit(0);
////        }
////        else {
////            print_string("error: expresion invalida", RED);
////            return;
////        }
////    }
////
////    // El primer string debe ser un programa valido
////    void* program_a =  get_program(tokens);
////    // Si no se encontro programa, entonces no es un string valido
////    if(program_a == NULL){
////        // Lanzar error: print rojo
////        print_string("error: programa invalido", RED);
////        return;
////    }
////    // Para iterar sobre los tokens, se debe hacer strtok(NULL, tokens), donde NULL es una constante igual a 0
////    tokens = strtok(0, tokens);
////    // Si solo habia un string (y valido) entonces se ejecuta un programa en toda la pantalla
////    if (tokens == NULL) {
////        uint64_t program[1] = {(uint64_t)program_a};
////        sys_exec(1, (void *)program);
////        return;
////    }
////    // En cambio, si hay mas de un string, el proximo debe ser un pipe
////    // Si no lo es, entonces es una expresion invalida
////    if(strcmp(tokens, "|") != 0) {
////        // Lanzar error: print rojo
////        print_string("error: no usa | como separador de programas", RED);
////        return;
////    }
////    tokens = strtok(0, tokens);
////    // Si no hay un tercer string tambien es invalido pues debe ser 'pgm1 | pgm2'
////    if(tokens == NULL){
////        // Lanzar error: print rojo
////        print_string("error: programa de consola derecha ausente", RED);
////        return;
////    }
////    // Luego, el tercer string debe ser un programa valido
////    void* program_b = get_program(tokens);
////    // Si no lo es lanza error
////    if (program_b == NULL) {
////        // Lanzar error: print rojo
////        print_string("error: programa para consola derecha invalido", RED);
////        return;
////    }
////    // Y no debe haber mas strings, para asi ejecutar dos programas
////    tokens = strtok(0, tokens);
////    if (tokens == NULL) {
////        void* program[2] = {program_a, program_b};
////        sys_exec(2, (void *)program);
////        return;
////    }
////    // Si los hay lanza error
////    else{
////        // Lanzar error: print rojo
////        print_string("error: cantidad de programas invalida", RED);
////        return;
////    }
////}
////
//
//int analyze_buffer(void) {
//    // Obtenemos los strings sin espacios de los ingresado en la terminal
//    char * tokens = str_tok(buffer, " ");
//    // Si no se ingreso texto, solo ENTER, no se hace nada
//    if(tokens == NULL){
//        return 0;
//    }
//    // Si se escribe "logout", bash se cierra y se apaga la compu
//    if(strcmp(tokens, LOGOUT) == 0){
//        tokens = str_tok(NULL, tokens);
//        // Verificamos que solo se halla ingresado "logout" y nada mas
//        if(tokens == NULL){
//            sys_exit(0);
//        }
//        else {
//            print_string("error: expresion invalida", RED);
//            return 0;
//        }
//    }
//    // El primer string debe ser un programa valido
//    void* program_a = get_program(tokens);
//    // Si no se encontro programa, entonces no es un string valido
//    if(program_a == NULL){
//        // Lanzar error: print rojo
//        print_string("error: programa invalido", RED);
//        return 0;
//    }
//    //logica exclusiva para printmem, si no es entonces no entra
//    if(program_a == get_program("printmem")){
//      tokens = str_tok(0, tokens);
//      if(tokens == NULL){
//          print_string("error: Ausencia de parametros en llamada a printmem", RED);
//          return 0;
//      }else{
//          //todo subirlo a la estruc del programa o pasarlo como parametro en sys_exec()
//          char * param = tokens;
//          uint64_t program[1] = {(uint64_t)program_a};
//          sys_exec(1, (void *) program_a);
//          return 1;
//      }
//    }
//    // Para iterar sobre los tokens, se debe hacer strtok(NULL, tokens), donde NULL es una constante igual a 0
//    tokens = str_tok(0, tokens);
//   // Si solo habia un string (y valido) entonces se ejecuta un programa en toda la pantalla
//    if (tokens == NULL) {
//        uint64_t program[1] = {(uint64_t)program_a};
//        sys_exec(1, (void *)program);
//       return 1;
//   }
//    // En cambio, si hay mas de un string, el proximo debe ser un pipe
//   // Si no lo es, entonces es una expresion invalida
//    if(strcmp(tokens, "|") != 0) {
//        // Lanzar error: print rojo
//        print_string("error: no usa | como separador de programas", RED);
//        return 0;
//    }
//    tokens = str_tok(0, tokens);
//    // Si no hay un tercer string tambien es invalido pues debe ser 'pgm1 | pgm2'
//    if(tokens == NULL){
//        // Lanzar error: print rojo
//        print_string("error: programa de consola derecha ausente", RED);
//        return 0;
//    }
//    // Luego, el tercer string debe ser un programa valido
//    void* program_b = get_program(tokens);
//    // Si no lo es lanza error
//    if (program_b == NULL) {
//        // Lanzar error: print rojo
//        print_string("error: programa para consola derecha invalido", RED);
//        return 0;
//    }
//    if(program_a == get_program("printmem")){
//      si es printmem entonces debo consumir un tokens mas para guardar el parametro
//      tokens = str_tok(0, tokens);
//      if(tokens == NULL){
//          print_string("error: Ausencia de parametros en llamada a printmem", RED);
//          return 0;
//      }else{
//          //todo subirlo a la estruc del programa o pasarlo como parametro en sys_exec()
//          char * param = tokens;
//          uint64_t program[2] = {(uint64_t)program_a};
//          sys_exec(2, (void *) program_a);
//          return 1;
//      }
//    }
//    // Y no debe haber mas strings, para asi ejecutar dos programas
//    tokens = str_tok(0, tokens);
//    if (tokens == NULL) {
//        void* program[2] = {program_a, program_b};
//        sys_exec(2, (void *)program);
//        return 1;
//    }
//    // Si los hay lanza error
//    else{
//        // Lanzar error: print rojo
//        print_string("error: cantidad de programas invalida", RED);
//        return 0;
//    }
//}
//
//
//////---------------------------------------------------------------------------------
////// clean_buffer: Limpia el buffer
//////---------------------------------------------------------------------------------
////// Argumentos:
//////   void
//////---------------------------------------------------------------------------------
////// Retorno
//////   void
//////---------------------------------------------------------------------------------
//void clean_buffer(void){
//    for(int i = 0; i < buffer_index; i++){
//        buffer[i] = '\0';
//    }
//    buffer_index = 0;
//}