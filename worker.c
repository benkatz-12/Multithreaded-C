#include "worker.h"
#include "shared-buffer.h"
#include "util.h"
void *producer(void * req_tid){

	prod_t * prod = (prod_t *)req_tid;
	printf("Thread id: %d\n", (int)*(prod -> tid));
	while(*(prod->count) < prod->numFiles){
		//printf("    GETTING FILE\n");
		pthread_mutex_lock(&prod->M); //CRITICAL AREA (choose next file)
			char* my_file = *(prod -> inputs + *(prod -> count));
			*(prod -> count) = (*prod -> count) + 1;
		pthread_mutex_unlock(&prod->M); //END CRITICAL AREA
		printf("    AQUIRED FILE: %s\n", my_file);
		FILE* fp;
		char buf[MAX_NAME_LENGTH];
		//char *a;
		if((fp = fopen(my_file, "r")) != NULL){
			while(fgets(buf, MAX_NAME_LENGTH, fp) != NULL){
				// a = fgets(buf, MAX_NAME_LENGTH, fp);
				// if(a == NULL){break;}


				if(buf[strlen(buf)-1] == '\n'){
					buf[strlen(buf)-1] = '\0';
				}
				//printf("    BEFORE ENQUEUE\n");
				enqueue(prod->queue, buf);
				//printf("    AFTER ENQUEUE\n");
				pthread_mutex_lock(&prod->M_log);//CRITICAL AREA (Print to log file)
					fprintf(prod->log, "%s\n", buf);
				pthread_mutex_unlock(&prod->M_log);//END CRITICAL AREA
				//printf("    AFTER LOG\n");
				memset(buf, 0, MAX_NAME_LENGTH);
				//printf("    AFTER MEMSET\n");
			}
		}else{
			printf("Error opening file %s\n", my_file);
		}
		fclose(fp);
	}
	return NULL;
};

void *consumer(void * res_tid){
	cons_t * cons = (cons_t *)res_tid;
	char x[MAX_NAME_LENGTH] = "";
	char ipv4[MAX_IP_LENGTH] = "";
	printf("Thread id: %d\n", (int)*(cons -> tid));
	while(1){
		memset(x, 0, MAX_NAME_LENGTH);
		if(*(cons->stop) == 1){
			break;
		}
		dequeue(cons->queue, x);

		if((strncmp(x, "12345678", 8) == 0) || *(cons->stop) == 1){
			*(cons->stop) = 1;
			break;
		}

		//printf("BEFORE DNS\n");
		if(dnslookup(x, ipv4, MAX_IP_LENGTH) != 0){
			//printf("    AFTER DNS\n");
			pthread_mutex_lock(&cons->M_log);
				fprintf(cons->log, "%s, %s\n", x, "NOT_RESOLVED");
			pthread_mutex_unlock(&cons->M_log);
		}else{
			//printf("    AFTER DNS\n");
			pthread_mutex_lock(&cons->M_log);
				fprintf(cons->log, "%s, %s\n", x, ipv4);
			pthread_mutex_unlock(&cons->M_log);
		}

	}
	 //sync error with this

	
	return NULL;
};
