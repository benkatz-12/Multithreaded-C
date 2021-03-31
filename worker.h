#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define ARRAY_SIZE 10
#define MAX_NAME_LENGTH 255
#define MAX_IP_LENGTH INET6_ADDRSTRLEN




void *producer(void * req_tid);
void *consumer(void * res_tid);

typedef struct producer_t{
	pthread_t * tid;
	pthread_mutex_t M; //for grabbing a new file
	pthread_mutex_t M_log; //for writing to log
	int * count;
	int numFiles;
	FILE * log;
	char ** inputs;
	struct queue* queue;
}prod_t;


typedef struct consumer_t{
	pthread_t * tid;
	pthread_mutex_t M_log; //for writing to log
	FILE * log;
	int * stop;
	struct queue* queue;
}cons_t;
