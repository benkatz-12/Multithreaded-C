#include "worker.h"
#include "shared-buffer.h"
#include "util.h"
void *producer(void * req_tid){

	prod_t * prod = (prod_t *)req_tid;

	while(*(prod->count) < prod->numFiles){

		pthread_mutex_lock(&prod->M); //CRITICAL AREA (choose next file)
			char* my_file = *(prod -> inputs + *(prod -> count));
			*(prod -> count) = (*prod -> count) + 1;
		pthread_mutex_unlock(&prod->M); //END CRITICAL AREA

		FILE* fp;
		char buf[MAX_NAME_LENGTH];

		if((fp = fopen(my_file, "r")) != NULL){
			while(fgets(buf, MAX_NAME_LENGTH, fp) != NULL){
				
				if(buf[strlen(buf)-1] == '\n'){
					buf[strlen(buf)-1] = '\0';
				}

				char * a = (char *)malloc(sizeof(char) * MAX_NAME_LENGTH);

				strncpy(a, buf, sizeof(buf));

				enqueue(prod->queue, &*a);
				pthread_mutex_lock(&prod->M_log);//CRITICAL AREA (Print to log file)
					fprintf(prod->log, "%s\n", buf);
				pthread_mutex_unlock(&prod->M_log);//END CRITICAL AREA
				memset(buf, 0, MAX_NAME_LENGTH);
				
			}
		}else{
			printf("invalid file %s\n", my_file);
		}
		prod->filesServiced++;
		fclose(fp);
	}
	printf("thread %d serviced %d files\n", (int)pthread_self(), prod->filesServiced);
	return NULL;
};

void *consumer(void * res_tid){
	cons_t * cons = (cons_t *)res_tid;
	char *x = "";
	char ipv4[MAX_IP_LENGTH] = "";
	while(1){
		x = dequeue(cons->queue);
		if((strncmp(x, "12345678", 8) == 0)){
			break;
		}

		if(dnslookup(x, ipv4, MAX_IP_LENGTH) != 0){
			pthread_mutex_lock(&cons->M_log);
				fprintf(cons->log, "%s, %s\n", x, "NOT_RESOLVED");
			pthread_mutex_unlock(&cons->M_log);
		}else{
			pthread_mutex_lock(&cons->M_log);
				fprintf(cons->log, "%s, %s\n", x, ipv4);
			pthread_mutex_unlock(&cons->M_log);
		}
		cons->numResolved++;
		free(x);
	}
	printf("thread %d resolved %d hostnames\n", (int)pthread_self(), cons->numResolved);
	return NULL;
};
