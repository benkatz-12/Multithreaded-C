#include "multi-lookup.h"
#include "worker.h"
#include "shared-buffer.h"
int main(int argc, char * argv[]){
	gettimeofday(&begin, NULL);
	//file pointers
	FILE* requester_log;
	FILE* resolver_log;
	
	//number of threads
	int num_req;
	int num_res;
	sscanf(argv[1], "%d", &num_req);
	sscanf(argv[2], "%d", &num_res);

	//put all file names into an array
	char *inputs[argc-5];
	int i;
	for(i = 0; i < argc - 5; i++){
		inputs[i] = argv[i+5];
	}
	char ** array_of_inputs = inputs;


	if(argc <= 5){ //argument checking
		printf(SYNOPSIS);
		exit(-1);
	}else if(argc > MAX_INPUT_FILES){ //argument checking
		ERROR("Too Many Input Arguments: MAX = 100\n");
		exit(-1);
	}else if(num_req > MAX_REQUESTER_THREADS){
		ERROR("Too Many Requester Threads Requested: MAX = 10\n");
		exit(-1);
	}else if(num_res > MAX_RESOLVER_THREADS){
		ERROR("Too Many Resolver Threads Requested: MAX = 10\n");
		exit(-1);
	}else{
		if((requester_log = fopen(argv[3], "w")) != NULL){
			if((resolver_log = fopen(argv[4], "w")) != NULL){
				spawn_threads(num_req, num_res, argc-5, array_of_inputs, requester_log, resolver_log);
				fclose(requester_log);
				fclose(resolver_log);
				gettimeofday(&end, NULL);
				double elapsed = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
				printf("./multi-lookup: total time is %f seconds\n", elapsed);
				exit(0);
			}else{
				ERROR("Resolver Log Creation Error\n")
			}
		}else{
			ERROR("Requester Log Creation Error\n");
		}
	}
}


void spawn_threads(int num_requester, int num_resolver, int numFiles, char ** inputs, FILE* requester_log, FILE* resolver_log){
	
	pthread_t req_tid[num_requester];
	pthread_t res_tid[num_resolver];
	pthread_mutex_t M = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t M_Resolver_log = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t M_Requester_log = PTHREAD_MUTEX_INITIALIZER;

	int count = 0;
	//initalize shared buffer
		pthread_mutex_t M_buf = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t out = PTHREAD_COND_INITIALIZER;
		pthread_cond_t in = PTHREAD_COND_INITIALIZER;
		
		struct queue q;
		struct queue *pt = &q;

		//initalize array
		char* items[ARRAY_SIZE];
		for(int m = 0; m<ARRAY_SIZE; m++){ 
			items[m] = "";
		}

		char ** a = items;
		char * pill = "12345678\0";
		char ** p = &pill;

		pt -> M_buf = M_buf;
		pt -> out_CV = out;
		pt -> in_CV = in;
		pt -> items = a;
		pt -> maxsize = ARRAY_SIZE;
		pt -> pill = p;
		pt -> front = 0;
		pt -> rear = -1;
		pt -> size = 0;

	//spawn threads
		//producer
			struct producer_t prod_t[num_requester];

			for(int i = 0; i < num_requester; i++){
				prod_t[i].M = M;
				prod_t[i].M_log = M_Requester_log;
				prod_t[i].inputs = inputs;
				prod_t[i].filesServiced = 0;
				prod_t[i].log = requester_log;
				prod_t[i].count = &count;
				prod_t[i].numFiles = numFiles;
				prod_t[i].queue = pt;
				if(pthread_create(&req_tid[i], NULL, producer, (void *)&prod_t[i]) != 0){
					ERROR("Pthread Create Error");
				}
			}
		//consumer
			struct consumer_t cons_t[num_resolver];
			for(int j = 0; j < num_resolver; j++){
				cons_t[j].log = resolver_log;
				cons_t[j].M_log = M_Resolver_log;
				cons_t[j].numResolved = 0;
				cons_t[j].queue = pt;
				///cons_t[j].stop = &stop;
				if(pthread_create(&res_tid[j], NULL, consumer, (void *)&cons_t[j]) != 0){
					ERROR("Pthread Create Error");	
				}
			}

	//join statements to end all threads
	for(int z = 0; z < num_requester; z++){
		pthread_join(req_tid[z], NULL);
	}
	for(int u = 0; u < num_resolver; u++){
		enqueue(pt, *pt->pill);
	}

	for(int x = 0; x < num_resolver; x++){
		pthread_join(res_tid[x], NULL);
	}
};
