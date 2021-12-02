#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>

static const int N_DUENDES = 10;
static const int N_RENAS = 9;

static int duendes;
static int renas;
static sem_t papaiNoelSem;
static sem_t renaSem;
static sem_t duendeTex;
static sem_t mutex;

// Método para criar as threads
pthread_t *CreateThread(void *(*f)(void *), void *a)
{
	pthread_t *t = malloc(sizeof(pthread_t));
	assert(t != NULL);
	int ret = pthread_create(t, NULL, f, a);
	assert(ret == 0);
	return t;
}

// Corpo da thread para o papai noel
void *PapaiNoelThread(void *arg)
{
	printf("Papai Noel: Hoho, aqui estou\n");
	while (true)
	{
		sem_wait(&papaiNoelSem);
		sem_wait(&mutex);

		if (renas == N_RENAS)
		{
			printf("Papai Noel: preparando o trenó\n");

			for (int r = 0; r < N_RENAS; r++)
			{
				sem_post(&renaSem);
			}

			printf("Papai Noel: faça todas as crianças do mundo felizes\n");
			renas = 0;
		}
		else if (duendes == 3)
		{
			printf("Papai Noel: ajudando duendes\n");
		}

		sem_post(&mutex);
	}
	return arg;
}

// Corpo da thread para as renas
void *RenaThread(void *arg)
{
	int id = (int)arg;
	printf("Essa é a rena %d\n", id);

	while (true)
	{
		sem_wait(&mutex);
		renas++;

		if (renas == N_RENAS)
		{
			sem_post(&papaiNoelSem);
		}

		sem_post(&mutex);
		sem_wait(&renaSem);
		printf("Rena %d sendo amarrada\n", id);
		sleep(20);
	}

	return arg;
}

// Corpo da thread para os duendes
void *DuendeThread(void *arg)
{
	int id = (int)arg;
	printf("Esse é o duende %d\n", id);

	while (true)
	{
		bool precisa_de_ajuda = random() % 100 < 10;

		if (precisa_de_ajuda)
		{
			sem_wait(&duendeTex);
			sem_wait(&mutex);
			duendes++;

			if (duendes == 3)
			{
				sem_post(&papaiNoelSem);
			}
			else
			{
				sem_post(&duendeTex);
			}

			sem_post(&mutex);

			printf("Duende %d receberá ajuda do papai noel\n", id);
			sleep(10);

			sem_wait(&mutex);
			duendes--;

			if (duendes == 0)
			{
				sem_post(&duendeTex);
			}

			sem_post(&mutex);
		}

		printf("Duende %d no trabalho\n", id);
		sleep(2 + random() % 5);
	}
	return arg;
}

int main()
{
	// Iniciando as variáveis
	duendes = 0;
	renas = 0;
	sem_init(&papaiNoelSem, 0, 0);
	sem_init(&renaSem, 0, 0);
	sem_init(&duendeTex, 0, 1);
	sem_init(&mutex, 0, 1);

	// Criando a thread para o papai noel
	pthread_t *papaiNoelSem = CreateThread(PapaiNoelThread, 0);

	// Criando a thread para as renas
	pthread_t *renas[N_RENAS];
	for (int r = 0; r < N_RENAS; r++)
		renas[r] = CreateThread(RenaThread, (void *)r + 1);

	// Criando a thread para os duendes
	pthread_t *duendes[N_DUENDES];
	for (int e = 0; e < N_DUENDES; e++)
		duendes[e] = CreateThread(DuendeThread, (void *)e + 1);

	int ret = pthread_join(*papaiNoelSem, NULL);
	assert(ret == 0);
}