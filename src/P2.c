#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include "customHeader.h"
#include "defined.h"

char **a;
typedef struct
{
	int sig[6];
	int q[x *z + 1];
	int r;
	int rv[x + 1];
	int cv[z + 1];
	int idx[4];
	pthread_mutex_t lock1;
	pthread_mutex_t lock2;
	int matrix1[x][y];
	int matrix2[y][z];
}

controller;
controller * cntlr;
int at = 0;
int **productMatrix;
typedef struct
{
	int num;
	int k;
}

arguments;
int done = 0;

void checkToStopThread()
{
	if (cntlr->sig[P2_SWITCH] == OFF)
	{
		pthread_mutex_lock(&cntlr->lock2);
		cntlr->sig[P2_THREAD_COUNT]--;
		pthread_mutex_unlock(&cntlr->lock2);
		while (cntlr->sig[P2_SWITCH] == OFF)
		{
			usleep(10);
		}

		pthread_mutex_lock(&cntlr->lock2);
		cntlr->sig[P2_THREAD_COUNT]++;
		pthread_mutex_unlock(&cntlr->lock2);
	}
}

void *matrixMultiplier(void *vargp)
{
	while (at == 0);
	arguments *arg = (arguments*) vargp;

	for (int o = 0; arg->num + o * threadForCalculatingProductMatrix < (x *z); o++)
	{
		int position = 0, elementSum = 0, i = 0, k = 0, t = 1;
		if ((arg->num + o *threadForCalculatingProductMatrix) >= cntlr->r)
		{
			pthread_mutex_lock(&cntlr->lock2);
			cntlr->sig[P2_THREAD_COUNT]--;
			pthread_mutex_unlock(&cntlr->lock2);
			while (cntlr->sig[P2_SWITCH] == ON)
			{
				usleep(10);
			}

			while (cntlr->sig[P2_SWITCH] == OFF)
			{
				usleep(10);
			}

			pthread_mutex_lock(&cntlr->lock2);
			cntlr->sig[P2_THREAD_COUNT]++;
			pthread_mutex_unlock(&cntlr->lock2);
			o--;
			continue;
		}
		else
		{
			position = cntlr->q[arg->num + o *threadForCalculatingProductMatrix];
			i = position / z;
			k = position % z;
			for (int j = 0; j < y; ++j)
			{
				elementSum += cntlr->matrix1[i][j] *cntlr->matrix2[j][k];
			}

			productMatrix[i][k] = elementSum;
			checkToStopThread();
		}
	}

	pthread_mutex_lock(&cntlr->lock2);
	cntlr->sig[P2_THREAD_COUNT]--;
	pthread_mutex_unlock(&cntlr->lock2);
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	a = argv;
	if (argc != 2)
	{
		perror("./P2.out out.txt\n");
		exit(0);
	}

	productMatrix = (int **) malloc(x* sizeof(int*));
	for (short int i = 0; i < x; i++)
	{
		productMatrix[i] = (int*) malloc(z* sizeof(int));
	}

	pthread_t *tid = (pthread_t*) malloc(x *z* sizeof(pthread_t));
	arguments *arg = (arguments*) malloc(x *z* sizeof(arguments));
	key_t key = ftok("Scheduler.c", 69);
	int shmid = shmget(key, sizeof(controller), 0666);
	if (shmid == -1)
	{
		perror("shmget failed");
		exit(0);
	}

	cntlr = (controller*) shmat(shmid, (void*) 0, 0);
	if (cntlr == (void*) - 1)
	{
		perror("shmat failed");
		exit(0);
	}

	for (int i = 0; i < threadForCalculatingProductMatrix; i++)
	{
		arg[i].num = i;
		pthread_create(&tid[i], NULL, matrixMultiplier, (void*) &arg[i]);
		cntlr->sig[P2_THREAD_COUNT]++;
	}

	at = 1;
	for (int i = 0; i < threadForCalculatingProductMatrix; i++)
	{
		pthread_join(tid[i], NULL);
	}

	FILE * filePointer;
	filePointer = fopen(a[1], "w");
	for (int i = 0; i < x; ++i)
	{
		for (int j = 0; j < z; ++j)
		{
			if (j + 1 == z)
			{
				fprintf(filePointer, "%d", productMatrix[i][j]);
			}
			else
			{
				fprintf(filePointer, "%d ", productMatrix[i][j]);
			}
		}

		fprintf(filePointer, "\n");
	}

	fclose(filePointer);
	cntlr->sig[P2_ENDED] = YES;
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}