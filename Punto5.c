

#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>			//Input Output

pthread_t *Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;        // contador de la silla
int CurrentIndex = 0;		// para llevar el apuntador de la silla

//Declaration of Semaphores and Mutex Lock.
sem_t TA_Sleep; 
sem_t Student_Sem;
sem_t ChairsSem[3]; // arreglo de semaforos
pthread_mutex_t ChairAccess; // acceso a las sillas

//Declared Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

	//Initializing Mutex Lock and Semaphores.
	sem_init(&TA_Sleep, 0, 0);  // el 0,0 indica q el proceso no se va a compartir y es unico para cada parametro
	sem_init(&Student_Sem, 0, 0);
	for(id = 0; id < 3; ++id)			//id cada silla
		sem_init(&ChairsSem[id], 0, 0);

	pthread_mutex_init(&ChairAccess, NULL); // si la silla es accesible o no
	
	if(argc<2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5; // si no se define un numero de estudiantes es 5 por defecto
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
		number_of_students = atoi(argv[1]); // si se indica se crean los hilas
	}
		
	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

	//Creating TA thread and N Student threads.

	pthread_create(&TA, NULL, TA_Activity, NULL);	
	for(id = 0; id < number_of_students; id++)
		pthread_create(&Students[id], NULL, Student_Activity,(void*) (long)id);



	//Waiting for TA thread and N Student threads.

	pthread_join(TA, NULL);
	for(id = 0; id < number_of_students; id++)
		pthread_join(Students[id], NULL);

	//Free allocated memory
	free(Students); 
	return 0;
}






void *TA_Activity()
{
	while(1) // para asegurar q siempre se ejecuta
	{
		sem_wait(&TA_Sleep);		//TA para saber si esta dormido
		printf("~~~~~~~~~~~~~~~~~~~~~TA has been awakened by a student.~~~~~~~~~~~~~~~~~~~~~\n");

		while(1)
		{
			// lock
			pthread_mutex_lock(&ChairAccess); // el asistente revisa las sillas mientras esta revisnado nadie puede entrar por lo que bloquea el acceso a estas

			if(ChairsCount == 0) 
			{
				//if chairs are empty, break the loop.
				pthread_mutex_unlock(&ChairAccess); // si hay sillas vacias desbloquea las sillas para q los estudiantes puedan entrar
				break;
			}


			//TA gets next student on chair.
			sem_post(&ChairsSem[CurrentIndex]);

			ChairsCount--; // atiuende un estudiante asi q vacia una de las sillas

			printf("Student left his/her chair. Remaining Chairs %d\n", 3 - ChairsCount);


			CurrentIndex = (CurrentIndex + 1) % 3;

			pthread_mutex_unlock(&ChairAccess);
			// unlock



			printf("\t TA is currently helping the student.\n");
			sleep(5); //aqui atiende al alumno
			sem_post(&Student_Sem);
			usleep(1000);
		}
	}
}

void *Student_Activity(void *threadID) 
{
	int ProgrammingTime; //  tiempo - programando

	while(1)
	{
		printf("Student %ld is doing programming assignment.\n", (long)threadID);
		ProgrammingTime = rand() % 10 + 1; //cuanto demora
		sleep(ProgrammingTime);		//tiempo q se demora programando 
		
		
		printf("Student %ld needs help from the TA\n", (long)threadID);
		
		pthread_mutex_lock(&ChairAccess); //bloquea aceso a la silla
		int count = ChairsCount; //guarda su posicion 
		pthread_mutex_unlock(&ChairAccess);

		if(count < 3)		//Student tried to sit on a chair.
		{
			if(count == 0)		//If student sits on first empty chair, wake up the TA.
				sem_post(&TA_Sleep);
			else
				printf("Student %ld sat on a chair waiting for the TA to finish. \n", (long)threadID);

			// lock
			pthread_mutex_lock(&ChairAccess); //bloquea aceso a la silla mientras se sienta
			int index = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;

			printf("Student sat on chair.Chairs Remaining: %d\n", 3 - ChairsCount);
			pthread_mutex_unlock(&ChairAccess); // desbloquea el acceso a las sillas ya que se sento
			// unlock

			sem_wait(&ChairsSem[index]);		//Student leaves his/her chair.
			printf("\t Student %ld is getting help from the TA. \n", (long)threadID);
			sem_wait(&Student_Sem);		//Student waits to go next.
			printf("Student %ld left TA room.\n",(long)threadID);
		}
		else 
			printf("Student %ld will return at another time. \n", (long)threadID);
			//If student didn't find any chair to sit on.
	}
}
