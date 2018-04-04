#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80        // Máxima longitud de un comando
#define BUFFER_SIZE 50     / Tamaño máximo del buffer donde se almacenarán temporalmente los comandos

char history[10][BUFFER_SIZE];  // Declaramos un arreglo bidimensional de caracteres donde guardaremos el historial de
                                // comandos

int count = 0;                  // Contador de comandos

void displayHistory(){
        printf("Shell command history:\n");     //Imprimimos información del historial
        int i;                                  //Instanciamos contador del for
        for (i = count - 1; i>-1; i--) {
                printf("%d.  %s", i + 1, history[i]);   // Imprimimos cada comando guardado en la historia
        }
}

int formatCommand(char inputBuffer[], char *args[],int *flag) {
        int length;   // Número de caracteres en la línea del comando
        int i;        // Índice del for
        int j;        // Segundo índice del for
        int start;    // Índice del comienzo del nuevo comando
        int ct = 0;   // Índice de dónde colocar el nuevo parámetro en args[]

        length = read(STDIN_FILENO, inputBuffer, MAX_LINE);   // Lectura

        if (length == 0) {                        //Si la longitud es 0, salimos
                exit(0); //end of command
        }
        if (length < 0) {                         //Si la longitud es menor a 1, salimos con error
                printf("Command not read\n");
                exit(-1); //terminate
        }

        if (inputBuffer[0] == '!') {                          // Si hay un !,
                if (inputBuffer[1] - '!' == 0) {              // Si hay otro !
                        for(i = 0; i < MAX_LINE; i++) {         // Copiamos la última entrada del historial
                                inputBuffer[i] = history[0][i]; // en la entrada para se procesada
                                if(inputBuffer[i] == '\0') {
                                        length = i + 1;         //Si encontramos un corte de línea, cortamos
                                        break;
                                }
                        }
                        inputBuffer[i] = '\0';                  //Y marcamos con el fin de la línea
                }else{                                          //Si no hay otro !
                        int x = inputBuffer[1] - '0';           //Esperamos un número y lo guardamos en x
                        if(x>count) {                           //Si el número es mayor al contador
                                printf("\nNo Such Command in the history\n"); //Imprimimos un error
                                return -1;
                        } else {
                                for(i = 0; i < MAX_LINE; i++) {
                                        inputBuffer[i] = history[x - 1][i];   //Si podemos copiar, copiamos
                                        if(inputBuffer[i] == '\0') {          //Hasta encontrar un corte de línea
                                                length = i;
                                                break;
                                        }
                                }
                                inputBuffer[i] = '\0';
                        }
                }
        }else{
                for (i = count + 1; i>0; i--) {                         //Si no hay un ! en el principio de la línea
                        strcpy(history[i], history[i-1]);               //copiamos la línea en el historial
                }
                for(j = 0; j < length; j++) {
                        history[0][j] = inputBuffer[j];
                }
                history[0][j] = '\0';
                count++;
                if(count > 10) {
                        count=10;
                }
        }
        start = -1;                               //Declaramos start -1
        for (i = 0; i < length; i++) {                //En otro caso, guardamos la lectura en args
                switch (inputBuffer[i]) {
                case ' ':
                case '\t':
                        if(start != -1) {
                                args[ct] = &inputBuffer[start];
                                ct++;
                        }
                        inputBuffer[i] = '\0';
                        start = -1;
                        break;
                case '\n':
                        if (start != -1) {
                                args[ct] = &inputBuffer[start];
                                ct++;
                        }
                        inputBuffer[i] = '\0';
                        args[ct] = NULL; // no more args
                        break;
                default:
                        if (start == -1)
                                start = i;
                        if (inputBuffer[i] == '&') {
                                *flag  = 1;
                                inputBuffer[i] = '\0';
                        }
                }
        }

        args[ct] = NULL;                                //Marcamos el fin de args
        if(args[0] != NULL) {                           //Si la línea no es nula, comparamos si
                if(strcmp(args[0],"history")==0) {      //el comando es igual a "history"
                        displayHistory();               //Mostramos el historial y retornamos con
                        return -1;                      //error para no realizar el comando.
                }
        }
        return 0;
}

int main(void) {
        char inputBuffer[MAX_LINE];     // Declaramos el bufer donde guardaremos la línea de comandos
        int flag;                       // Declaramos una variable que nos dirá si se quiere correr el comando en background
        char *args[MAX_LINE/2 + 1];     // Declaramos el buffer donde guardaremos el comando y sus parámetros en
                                        // el formato que recive execvp
        int should_run = 1;             // Declaramos una variable que dos dirá cuándo seguir corriendo

        pid_t pid;                      // Declaramos los identificadores de los procesos (padre e hijo)

        while (should_run) {            // Loop infinito, seguirá según sigue siendo 1 should_run
                flag = 0;               // Por defecto, no queremos correr un commando en background
                printf("osh>");         // Imprimimos el nombre de la shell
                fflush(stdout);         // Se limpia la salida estándar para evitar problemas de lectura
                if(-1!=formatCommand(inputBuffer,args,&flag)) { // Obtenemos el siguiente comando con la función formatCommand,
                                                                // con ella, en inputBuffer queda guardado el comando como
                                                                // cadena de carácteres, en args, queda guardado en el formato
                                                                // que recive execvp y flag nos indicará si queremos correr en
                                                                // background.

                        pid = fork();                           // Relizamos el fork
                        if (pid < 0) {                          // Si tenemos un pid menor a 0, hubo un error
                                printf("Fork failed.\n");
                                exit (1);
                        } else if (pid == 0) {                                // Si tenemos un pid igual a 0, estamos en el proceso hijo
                                if (execvp(args[0], args) == -1) {            // Así que intentamos realizar el execvp, si es igual a -1
                                        printf("Error executing command\n");  // hubo un error
                                }
                        }
                        else {                              // Si tenemos otro pid, estamos en el proceso padre
                                if (flag == 0) {            // Si la bandera sigue siendo 0, por defecto, esperamos al proceso
                                        wait(NULL);         // hijo a que termine.
                                }
                        }
                }
        }
}

