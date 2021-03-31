#include "shared-buffer.h"
#include <errno.h>
extern int errno;
void showarray(struct queue* pt);

int size(struct queue* pt){
	return pt -> size;
}

int isEmpty(struct queue* pt){
	return !size(pt);
}

char * front(struct queue* pt){
	//return pt->items[pt->front];
	return pt->items + (pt->front * MAX_NAME_LENGTH);
}

void enqueue(struct queue* pt, char *x){
	pthread_mutex_lock(&pt->M_buf);
		while(size(pt)==pt->maxsize){pthread_cond_wait(&pt->out_CV, &pt->M_buf);}
		//printf("inserting %s\n", x);

		pt->rear = (pt->rear + 1) % pt-> maxsize;
		//strcpy(pt->items[pt->rear], x);
		strncpy(pt->items + (pt->rear * MAX_NAME_LENGTH), x, strlen(x));
		pt->size++;
		//showarray(pt);
	pthread_cond_signal(&pt->in_CV);
	pthread_mutex_unlock(&pt->M_buf);
	
}

void dequeue(struct queue* pt, char* y){
	char* x;
	pthread_mutex_lock(&pt->M_buf);
		//printf("IN DEQUEUE\n");
		while(size(pt) == 0){pthread_cond_wait(&pt->in_CV, &pt->M_buf);}
		//printf("removing %s\n", front(pt));
		x = front(pt);
		pt->front = (pt->front + 1) % pt->maxsize;
		pt->size--;
		strncpy(y, x, strlen(x));
		memset(x, 0, 50);
		//showarray(pt);
	pthread_mutex_unlock(&pt->M_buf);
	pthread_cond_signal(&pt->out_CV);
	//printf("OUT OF DEQUEUE\n");
}

struct queue* newQueue(int size){
	pthread_mutex_t M_buf;
	pthread_cond_t out;
	pthread_cond_t in;
	
	if(pthread_mutex_init(&M_buf, NULL) != 0){
		printf("Mutex Error");
	}else if(pthread_cond_init(&out, NULL) != 0){
		printf("CV Error");
	}else if(pthread_cond_init(&in, NULL) != 0){
		printf("CV Error");
	}
	
	struct queue *pt = NULL;
	pt = (struct queue*)malloc(sizeof(struct queue));
	pt -> M_buf = M_buf;
	pt -> out_CV = out;
	pt -> in_CV = in;
	pt -> items = (char **)calloc(size, sizeof(char[MAX_NAME_LENGTH]));
	//pt -> items = (char **)malloc(size * sizeof(char[MAX_NAME_LENGTH]));
	pt -> maxsize = size;
	pt -> pill = "12345678\0";
	pt -> front = 0;
	pt -> rear = -1;
	pt -> size = 0;

	return pt;
};
//
void cleanup(struct queue* pt){
	free(pt->items);
	free(pt);
}

void showarray(struct queue* pt){
	printf("front = %d, rear = %d\n", pt->front, pt->rear);

	printf("[0]  :  %s\n", pt->items + (0 * MAX_NAME_LENGTH));
	// printf("[1]  :  %s\n", pt->items + (1 * MAX_NAME_LENGTH));
	// printf("[2]  :  %s\n", pt->items + (2 * MAX_NAME_LENGTH));
	// printf("[3]  :  %s\n", pt->items + (3 * MAX_NAME_LENGTH));
	// printf("[4]  :  %s\n", pt->items + (4 * MAX_NAME_LENGTH));
	// printf("[5]  :  %s\n", pt->items + (5 * MAX_NAME_LENGTH));
	// printf("[6]  :  %s\n", pt->items + (6 * MAX_NAME_LENGTH));
	// printf("[7]  :  %s\n", pt->items + (7 * MAX_NAME_LENGTH));
	// printf("[8]  :  %s\n", pt->items + (8 * MAX_NAME_LENGTH));
	// printf("[9]  :  %s\n", pt->items + (9 * MAX_NAME_LENGTH));


	// printf("[0]  :  %s", pt->items[0]);
	// printf("[1]  :  %s", pt->items[1]);
	// printf("[2]  :  %s", pt->items[2]);
	// printf("[3]  :  %s", pt->items[3]);
	// printf("[4]  :  %s", pt->items[4]);
	// printf("[5]  :  %s", pt->items[5]);
	// printf("[6]  :  %s", pt->items[6]);
	// printf("[7]  :  %s", pt->items[7]);
	// printf("[8]  :  %s", pt->items[8]);
	// printf("[9]  :  %s", pt->items[9]);
}



