#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
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
} controller;

controller * cntlr;

double diff(struct timespec start, struct timespec end)
{
	double d;
	d = (long)end.tv_sec - (long)start.tv_sec;
	d += ((double)end.tv_nsec - (double)start.tv_nsec)/1000000000;
	return d;
}

void create_q()
{
	cntlr->idx[0] = cntlr->idx[2];
	cntlr->idx[1] = cntlr->idx[3];
	int e;
	for (e = cntlr->idx[0] + 1; e < x+1; e++)
	{
		if (cntlr->rv[e] != 1)
			break;
	}

	cntlr->idx[2] = e - 1;
	for (e = cntlr->idx[1] + 1; e < z+1; e++)
	{
		if (cntlr->cv[e] != 1)
			break;
	}

	cntlr->idx[3] = e - 1;
	for (int k = 0; k <= cntlr->idx[0]; k++)
	{
		for (int p = cntlr->idx[1] + 1; p <= cntlr->idx[3]; p++)
		{
			cntlr->q[cntlr->r++] = (p + (k *z));
		}
	}

	for (int k = cntlr->idx[0] + 1; k <= cntlr->idx[2]; k++)
	{
		for (int p = 0; p <= cntlr->idx[3]; p++)
		{
			cntlr->q[cntlr->r++] = (p + (k *z));
		}
	}
}

int main(int argc, char *argv[])
{
	printf("Program started\n");
	struct timespec ts1, te1, ts2, te2, ws1, we1, ws2, we2, ref;
	clock_gettime(CLOCK_REALTIME, &ref);
	double t1 = 0,t2 = 0, w1 = 0, w2 = 0, c = 0;
	// CLOCK_PER_SEC
	if (argc != 4)
	{
		perror("./S.out in1.txt in2.txt out.txt");
		exit(0);
	}

	key_t key = ftok("Scheduler.c", 69);
	int shmid = shmget(key, sizeof(controller), 0666 | IPC_CREAT);
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

	int j = 0;
	while (j < 6)
		cntlr->sig[j++] = 0;
	j = 0;
	while (j < 4)
		cntlr->idx[j++] = -1;
	cntlr->sig[P1_SWITCH] = ON;	//allowing p1 to start computations
	if (fork() == 0)
	{
		char *args[] = { "./P1.out", argv[1], argv[2], NULL };
		execv(args[0], args);
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &ts1);
		printf("P1 started at: %0.9lf\n", diff(ref,ts1));
		usleep(QUANTA);
		cntlr->sig[P1_SWITCH] = OFF;	// signaling p1 to pause
		while (cntlr->sig[P1_THREAD_COUNT] != 0);	// wait until p1 threads stops
		clock_gettime(CLOCK_REALTIME, &ws1);
		printf("P1 paused at: %0.9lf\n", diff(ref,ws1));
		cntlr->r = 0;
		create_q();
	//	printf("%d\n", cntlr->r);
		cntlr->sig[P2_SWITCH] = ON;	//allowing p2 to start computations
		if (fork() == 0)
		{
			char *args[] = { "./P2.out", argv[3], NULL };
			execv(args[0], args);
		}
		else
		{
			clock_gettime(CLOCK_REALTIME, &ts2);
			printf("P2 started at: %0.9lf\n", diff(ref,ts2));
			c += diff(ws1, ts2);
			usleep(QUANTA);
			cntlr->sig[P2_SWITCH] = OFF;	//signaling p2 to pause
			while (cntlr->sig[P2_THREAD_COUNT] != 0);	// wait until threads in p2 stops
			clock_gettime(CLOCK_REALTIME, &ws2);
			printf("P2 paused at: %0.9lf\n", diff(ref,ws2));
		}
	}

	int p = 1;
	while (cntlr->sig[P1_ENDED] == NO && cntlr->sig[P2_ENDED] == NO)
	{
		// loop until one of the processes ends
		cntlr->sig[P1_SWITCH] = ON;	// signaling p1 to resume
		clock_gettime(CLOCK_REALTIME, &we1);
		printf("P1 resumed at: %0.9lf\n", diff(ref,we1));
		w1 += diff(ws1, we1);
		c += diff(we1, ws2);
		usleep(QUANTA);
		cntlr->sig[P1_SWITCH] = OFF;	//signaling p1 to pause
		while (cntlr->sig[P1_THREAD_COUNT] != 0 && cntlr->sig[P1_ENDED] == NO);	// wait until p1 stops
		clock_gettime(CLOCK_REALTIME, &ws1);
		printf("P1 paused at: %0.9lf\n", diff(ref,ws1));
		create_q();
		//printf("%d\n", cntlr->r);
		cntlr->sig[P2_SWITCH] = ON;	// signaling p2 to resume
		clock_gettime(CLOCK_REALTIME, &we2);
		printf("P2 resumed at: %0.9lf\n", diff(ref,we2));
		w2 += diff(ws2, we2);
		c += diff(ws1, we2);
		usleep(QUANTA);
		cntlr->sig[P2_SWITCH] = OFF;	// siganling p2 to pause
		while (cntlr->sig[P2_THREAD_COUNT] != 0 && cntlr->sig[P2_ENDED] == NO);	// wait until p2 stops
		clock_gettime(CLOCK_REALTIME, &ws2);
		printf("P2 paused at: %0.9lf\n", diff(ref,ws1));
	}

	if (cntlr->sig[P1_ENDED] == NO)
		cntlr->sig[P1_SWITCH] = ON;
	while (cntlr->sig[P1_ENDED] == NO);	
	clock_gettime(CLOCK_REALTIME, &te1);
	printf("P1 ended at: %0.9lf\n", diff(ref,te1));
	if (cntlr->sig[P2_ENDED] == NO)
		cntlr->sig[P2_SWITCH] = ON;
	while (cntlr->sig[P2_ENDED] == NO);
	clock_gettime(CLOCK_REALTIME, &te2);
	printf("P2 ended at: %0.9lf\n", diff(ref,te2));
	
	t1 = diff(ts1, te1);
	t2 = diff(ts2, te2);
	// printf("%0.9lf,%0.9lf,%0.9lf,%0.9lf,%0.9lf\n",t1,w1,t2,w2,c);
	printf("Program ended\n");
	shmdt(cntlr);
}