#include "shared-buffer.h"

void showarray(struct queue* pt);

int size(struct queue* pt){
	return pt -> size;
}

char * front(struct queue* pt){
	return pt->items[pt->front];
}

void enqueue(struct queue* pt, char *x){
	pthread_mutex_lock(&pt->M_buf);
		while(size(pt)==pt->maxsize){pthread_cond_wait(&pt->out_CV, &pt->M_buf);}
		pt->rear = (pt->rear + 1) % pt-> maxsize;
		pt->items[pt->rear] = &*x;
		pt->size++;
	pthread_cond_signal(&pt->in_CV);
	pthread_mutex_unlock(&pt->M_buf);
	
}

char* dequeue(struct queue* pt){
	char* x;
	pthread_mutex_lock(&pt->M_buf);
		while(size(pt) == 0){pthread_cond_wait(&pt->in_CV, &pt->M_buf);}
		x = front(pt);
		pt->front = (pt->front + 1) % pt->maxsize;
		pt->size--;
	pthread_mutex_unlock(&pt->M_buf);
	pthread_cond_signal(&pt->out_CV);
	return x;

}