#include "include/libc.h"

char ** programs_names = {"help",
                          "div0",
                          "invalid opcode",
                          "inforeg",
                          "printmem",
                          "hora",
                          "primos",
                          "fibonacci"};

void analyze_buffer();

char buffer[100];
int buffer_index = 0;

// No puedo compilar .asm ni .h en userspace, entonces no encuentra las funciones
// por eso todo esto horrible.
// mañana le preguntamos a horacio/joaquin como modificar el makefile
// igual nada de esto se esta corriendo asi que no molesta por ahora
uint8_t sys_read(char* c){}

uint8_t sys_write(char* string, uint8_t format){}

//---------------------------------------------------------------------------------
// getChar: lectura de un caracter con sys_call de lectura
//---------------------------------------------------------------------------------
// Argumentos:
//      int getChar();
//---------------------------------------------------------------------------------
// Retorno:
//      caracter leido o -1 si hubo error en la lectura
//---------------------------------------------------------------------------------
int getChar(){
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
// strcmp: comparar dos string
//---------------------------------------------------------------------------------
// Argumentos:
//      int strcmp(char* s1, char* s2)
//---------------------------------------------------------------------------------
// Retorno
//   s1 - s2 =>
//   s1 = s2 retorno 0
//   s1 > s2 retorno positivo
//   s1 < s2 retorno negativo
//---------------------------------------------------------------------------------
int strcmp(char* s1, char* s2){
    int i;
    for(i=0; s1[i]!='\0' && s2[i]!='\0' && s1[i]==s2[i]; i++);
    return s1[i]-s2[i];
}


char * get_programs_names(int index){
    return programs_names[index];
}





