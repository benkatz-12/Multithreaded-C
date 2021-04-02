#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>


#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define SYNOPSIS "MULTI-LOOKUP USAGE: \n multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]\n\n<#requesters> number of requestor threads to place into the thread pool\n <#resolvers> number of reslver threads to place into the thread pool\n <requester log> name of file into which requested hostnames are written\n <resolver log> name of the file into which hostnames and resolved IP addresses are written\n <data file> filename to be processed. Each file contains a list of host names, one per line, that are to be resolved\n"
#define ERROR(e) fprintf(stderr, e);

void spawn_threads(int num_requester, int num_resolver, int numfiles, char ** inputs, FILE* requester_log, FILE* resolver_log);
struct timeval begin, end;
