#include <bash.h>

char buffer[100];
int buffer_index = 0;
const int CANT_PROG = 7;

void analyze_buffer(void);

void bash(void){
    int c=getChar();
    if(c=='\n')
        analyze_buffer();
    if(c!=-1){
        buffer[buffer_index++]=c;
        buffer[buffer_index+1]='\0';
    }
}

void analyze_buffer(void){
    char ** tokens = strtok(buffer, ' ');
    char ** prog_names = get_program_names();
    int i = 0;
    for(; i < CANT_PROG && strcmp(tokens[0], prog_names[i]) != 0; i++);
    if(i >= CANT_PROG)
        // Lanzar error: print rojo
        sys_write("error", RED);
        return;
    for(; i < CANT_PROG && strcmp(tokens[1], prog_names[i]) != 0; i++);
    if(i >= CANT_PROG)
        // Lanzar error: print rojo
        sys_write("error", RED);
    return;

}