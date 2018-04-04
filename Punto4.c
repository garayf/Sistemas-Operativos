#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>

/*Example Of a Valid Sudoku Given in Book*/
int sud[9][9]={
                {6,2,4,5,3,9,1,8,7},
                {5,1,9,7,2,8,6,3,4}, 
                {8,3,7,6,1,4,2,9,5},
                {1,4,3,8,6,5,7,2,9},
                {9,5,8,2,4,7,3,6,1},
                {7,6,2,3,9,1,4,5,8},
                {3,7,1,9,5,6,8,4,2},
                {4,9,6,1,8,2,5,7,3},
                {2,8,5,4,7,3,9,1,6}
             };
             
             
             
// estructura de los datos que se pasaran 
struct data
{
int row;// cantidad de filas 
int col;//cantidad e columnas
};


/*Threads for Checking row and column*/
void *row_col(void *d)
{
long n,m,h=0;
m=(long long)d;
int k,i,j;

for(i=0;i<9;i++){
        k=1; // variable comparadora por medio de esto se comprueban los numeros del 1 al 9
         while(k<10){
            for(j=0;j<9;j++){ 
                if(m==0 && sud[i][j]==k){ //comparacion de filas se comprueba q en toda la fila hay numeros del 1 a 9
                          h++; // si se encuentra el numero se incrementa el contador en 1
                          goto aam; //ya que se encontro el numero evitamos el resto del for
                 }
                 else if(m==1 && sud[j][i]==k){ //comparacion de columnas sigue la misma logica anterior
                          h++;
                          goto aam;
                  }
             }
             aam:
             k++;
         }
}
if(h==81)// si el numero del contador es 81 quiere decir que se encontraron los 81 numeros 
{
n=1; // funciona como booleano true
}
else
{
n=-1; //funciona como booleano false
}
pthread_exit((void*)n); // se cierran los hilos para esta ejecucion
}


/*Threads for Checking each  square*/

void *sq_check(void *mn)
{
struct data *my_data=(struct data *)mn;
int i=(*my_data).row; //se pasa el valor de la estructura de los datos
int j=(*my_data).col;
int n,m,p;
n=i+3; //+3 para hacer el bloque de 3*3
m=j+3; //+3 para hacer el bloque de 3*3
int k,h=0;
for(k=1;k<10;k++){ //valor de comprobacion 1 al 9
    for(i=0;i<n;i++){ 
        for(j=0;j<m;j++){
            if(sud[i][j]==k){
                    h++;
                    i=n;j=m;
                }
         }
    }

}
if(h==9){//si el comparador es 9 todos los numeros se encontrraron
p=1;//sirve co0mo booleano true
}
else{
p=-1;//sirve como booleano false
}
pthread_exit((void*)p);
}
int main()
{
struct data *p;
p=(struct data*)malloc(sizeof(struct data));
pthread_t thread[11];
int i,a;
void *b;
int t,l,k;
for(i=0;i<2;i++)
{
t=i;
/* Creation of Threads for Checking row and column */
a=pthread_create(&thread[i],NULL,row_col,(void*)t); //se crean los hilos para comprobar filas y columnas
if(a)
{
printf("error"); //si algo sale mal imprime un error
}
}


/* Creation of Threads for Checking 9 squares */
for(l=0;l<=6;l=l+3){
    for(k=0;k<=6;k=k+3){ // se crean los hilos para los bloques +3 asi se analizan 3 bloques
            (*p).row=l;
            (*p).col=k;

            a=pthread_create(&thread[i++],NULL,sq_check,(void*)p);
              if(a){
                printf("error"); //se imprime error si hay algo mal
               }
     }
}

int s=0;
for(i=0;i<11;i++){
pthread_join(thread[i],&b);
s=s+(long long)b; //se suman los resultados obtenidos
}
if(s==11){// si todos los resultados son validos el numero es 11
printf("THE GIVEN SUDOKU IS VALID\n");
}
else{
printf("THE GIVEN SUDOKU IS INVALID\n");
}
pthread_exit(NULL);
}

