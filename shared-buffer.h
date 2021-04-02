#include <pthread.h>
#define ARRAY_SIZE 10
#define MAX_NAME_LENGTH 255

struct queue {
	char **items;
	int maxsize;
	int front;
	int rear;
	char **pill;
	int size;
	pthread_mutex_t M_buf;
	pthread_cond_t out_CV;
	pthread_cond_t in_CV;
};

int size(struct queue* pt);
char * front(struct queue* pt);
void enqueue(struct queue* pt, char *x);
char* dequeue(struct queue* pt);
struct queue* newQueue(int size);