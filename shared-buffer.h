#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#define ARRAY_SIZE 10
#define MAX_NAME_LENGTH 255
struct queue {
	char **items;
	//char * items[ARRAY_SIZE];
	int maxsize;
	int front;
	int rear;
	char *pill;
	int size;
	pthread_mutex_t M_buf;
	pthread_cond_t out_CV;
	pthread_cond_t in_CV;
};

int size(struct queue* pt);
int isEmpty(struct queue* pt);
char * front(struct queue* pt);
void enqueue(struct queue* pt, char *x);
void dequeue(struct queue* pt, char * y);
struct queue* newQueue(int size);
void cleanup(struct queue* pt);
