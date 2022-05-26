#include <libc.h>

char * programs_names[CANT_PROG] = {"help",
                          "div0",
                          "invalid opcode",
                          "inforeg",
                          "printmem",
                          "hora",
                          "primos",
                          "fibonacci"};

programs programs_list[CANT_PROG] = {help,
                            zero_division_exc,
                            invalid_opcode_exc,
                            inforeg,
                            printmem,
                            hora,
                            primos,
                            fibonacci};


void number_to_string(uint64_t number, char * str);

//---------------------------------------------------------------------------------
// getChar: lectura de un caracter con sys_call de lectura
//---------------------------------------------------------------------------------
// Argumentos:
//      int getChar();
//---------------------------------------------------------------------------------
// Retorno:
//      caracter leido o -1 si hubo error en la lectura
//---------------------------------------------------------------------------------
uint8_t get_char(void){
    char c;
    int ret = sys_read(&c);
    if(ret == 0) //Si no leyo caracteres
        return -1;
    return c;
}

//---------------------------------------------------------------------------------
// scanf: lectura de entrada en un buffer
//---------------------------------------------------------------------------------
// Argumentos:
//      void scanf()
//---------------------------------------------------------------------------------
// Retorno:
//---------------------------------------------------------------------------------
//void scanf(){
//    int c=getChar();
//    if(c=='\n')
//        analyze_buffer();
//    if(c!=-1){
//        buffer[buffer_index++]=c;
//        buffer[buffer_index+1]='\0';
//    }
//}

//---------------------------------------------------------------------------------
// printString: imprime un String
//---------------------------------------------------------------------------------
// Argumentos:
//      void printString(char * s1)
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_string(const char * s1, formatType format){
    return sys_write(s1, format);
}

//---------------------------------------------------------------------------------
// printNumber: imprime un Numero
//---------------------------------------------------------------------------------
// Argumentos:
//      void printString(int number, int format)
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
uint8_t print_number(uint64_t number, formatType format){
    char str[21];
    number_to_string(number, str);
    return print_string(str, format);
}

//---------------------------------------------------------------------------------
// number_to_string: Devuelve un string del numero pasado por parametros
//---------------------------------------------------------------------------------
// Argumentos:
//   number: El numero a convertir
//   str: El string sobre el cual copiar
//---------------------------------------------------------------------------------
// Retorno:
//      cantidad de caracteres escritos
//---------------------------------------------------------------------------------
void number_to_string(uint64_t num, char * str){
    // Si el numero es 0, devuelvo el string 0 y ya esta
    if(num == 0){
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    int i = 0, sign = 1;
    // Si el numero es negativo, tengo que al final agregarle el simbolo - al string
    if(num < 0){
        sign = -1;
        // Lo pongo positivo para facilitar la operacion
        num *= -1;
    }
    // Recorro hasta que num valga 0
    while(num > 0){
        int j = i;
        // Como leo del mas pequeño al mas grande, debo ir corriendo los digitos hacia la derecha: 1 2 3 -> _ 1 2 3
        while(j > 0){
            str[j] = str[j-1];
            j--;
        }
        // Obtengo el proximo digito y le sumo 30h para obtener su ascii
        str[0] = (num % 10) + 0x30;
        num /= 10;
        i++;
    }
    // Si el numero era negativo
    if(sign == -1){
        int j = i;
        // Vuelvo a correr el numero para poder insertar el -
        while(j > 0){
            str[j] = str[j-1];
            j--;
        }
        str[0] = '-';
        i++;
    }
    // Y agrego el \0 al final del string
    str[i] = '\0';
}


//---------------------------------------------------------------------------------
// strcmp: comparar dos string
//---------------------------------------------------------------------------------
// Argumentos:
//   s1: El primer string
//   s2: El segundo string
//---------------------------------------------------------------------------------
// Retorno
//   s1 - s2 =>
//   s1 = s2 retorno 0
//   s1 > s2 retorno positivo
//   s1 < s2 retorno negativo
//---------------------------------------------------------------------------------
uint64_t strcmp(const char* s1, const char* s2){
    int i;
    for(i=0; s1[i]!='\0' && s2[i]!='\0' && s1[i]==s2[i]; i++);
    return s1[i]-s2[i];
}

//---------------------------------------------------------------------------------
// get_program: Devuelve el programa asociado al string str
//---------------------------------------------------------------------------------
// Argumentos:
//   - el nombre del programa a ejecutar
//---------------------------------------------------------------------------------
// Retorno
//   - el puntero al programa a ejecutar
//---------------------------------------------------------------------------------
void* get_program(const char * str){
    for(int i = 0; i<CANT_PROG;i++){
        if(strcmp(str,programs_names[i])==0){
                return programs_list[i];
        }
    }
    // Si el string no es el de un programa, se devuelve NULL
    return NULL;
}







