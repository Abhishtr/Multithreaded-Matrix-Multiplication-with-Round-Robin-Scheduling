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

int athread = 0;
typedef struct
{
	int lineNumber;
}

rowReader;
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
char **a;
void checkToStopThread(int *i, int num, int c)
{
	if (cntlr->sig[P1_SWITCH] == OFF)
	{
		pthread_mutex_lock(&cntlr->lock1);
		cntlr->sig[P1_THREAD_COUNT]--;
		pthread_mutex_unlock(&cntlr->lock1);
		int j = 0;
		while (cntlr->sig[P1_SWITCH] == OFF)
		{
			usleep(10);
		}

		pthread_mutex_lock(&cntlr->lock1);
		cntlr->sig[P1_THREAD_COUNT]++;
		pthread_mutex_unlock(&cntlr->lock1);
	}
}

void *readFunction1(void *vargp)
{
	while (athread == 0);
	rowReader *arg = (rowReader*) vargp;
	char seps[] = " ";
	char *token;
	int
	var;
	int npause = 0;
	int num = arg->lineNumber;
	char line[1000];
	FILE *fp = fopen(a[1], "r");
	for (int j = 0; j < num; ++j)
	{
		fscanf(fp, "%[^\n]\n", line);
	}

	for (int i = num; i < x;)
	{
		int index = 0;
		fscanf(fp, "%[^\n]\n", line);
		char *rest = line;
		cntlr->rv[i] = 1;
		for (token = strtok_r(line, seps, &rest); token != NULL; token = strtok_r(NULL, seps, &rest))
		{
			sscanf(token, "%d", &var);
			cntlr->matrix1[i][index] =
				var;
			index++;;
		}

		++i;
		for (int j = 0; j < threadForReadingMatrix1 - 1 && i < x; ++j, ++i)
		{
			fscanf(fp, "%[^\n]\n", line);
		}

		checkToStopThread(&npause, num, 1);
	}

	pthread_mutex_lock(&cntlr->lock1);
	cntlr->sig[P1_THREAD_COUNT]--;
	pthread_mutex_unlock(&cntlr->lock1);
	fclose(fp);
	pthread_exit(0);
}

void *readFunction2(void *vargp)
{
	while (athread == 0);
	int np2 = 0;
	rowReader *arg = (rowReader*) vargp;
	char seps[] = " ";
	char *token;
	int
	var;
	int num = arg->lineNumber;
	char line[1000];
	FILE *fp = fopen(a[2], "r");
	for (int j = 0; j < num; ++j)
	{
		fscanf(fp, "%[^\n]\n", line);
	}

	for (int i = num; i < z;)
	{
		int index = 0;
		fscanf(fp, "%[^\n]\n", line);
		char *rest = line;
		cntlr->cv[i] = 1;
		for (token = strtok_r(line, seps, &rest); token != NULL; token = strtok_r(NULL, seps, &rest))
		{
			sscanf(token, "%d", &var);
			cntlr->matrix2[index][i] =
				var;
			index++;
		}

		++i;
		for (int j = 0; j < threadForReadingMatrix2 - 1 && i < y; ++j, ++i)
		{
			fscanf(fp, "%[^\n]\n", line);
		}

		checkToStopThread(&np2, num, 2);
		np2++;
	}

	pthread_mutex_lock(&cntlr->lock1);
	cntlr->sig[P1_THREAD_COUNT]--;
	pthread_mutex_unlock(&cntlr->lock1);
	fclose(fp);
	pthread_exit(0);
}

int main(int argc, char *argv[])
{
	a = argv;
	if (argc != 3)
	{
		perror("./P1.out in1.txt in2.txt\n");
		exit(0);
	}

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

	cntlr->r = 0;
	rowReader object1[threadForReadingMatrix1];
	for (int i = 0; i < threadForReadingMatrix1; ++i)
	{
		object1[i].lineNumber = i;
	}

	pthread_t tid1[threadForReadingMatrix1];
	for (int i = 0; i < threadForReadingMatrix1; ++i)
	{
		pthread_create(&tid1[i], NULL, readFunction1, (void*) &object1[i]);
		cntlr->sig[P1_THREAD_COUNT]++;
	}

	rowReader object2[threadForReadingMatrix2];
	for (int i = 0; i < threadForReadingMatrix2; ++i)
	{
		object2[i].lineNumber = i;
	}

	pthread_t tid2[threadForReadingMatrix2];
	for (int i = 0; i < threadForReadingMatrix2; ++i)
	{
		pthread_create(&tid2[i], NULL, readFunction2, (void*) &object2[i]);
		cntlr->sig[P1_THREAD_COUNT]++;
	}

	athread = 1;
	for (int i = 0; i < threadForReadingMatrix1; ++i)
	{
		if (pthread_join(tid1[i], NULL) != 0)
		{
			perror("Error in joining the thread\n");
			return 2;
		}
	}

	for (int i = 0; i < threadForReadingMatrix2; ++i)
	{
		if (pthread_join(tid2[i], NULL) != 0)
		{
			perror("Error in joining the thread\n");
			return 2;
		}
	}

	cntlr->sig[P1_ENDED] = YES;
	return 0;
}