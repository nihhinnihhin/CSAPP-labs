/* author: MCX;		ID: hinmo */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "cachelab.h"
// local, static,
// global variables 
int hitCount, missCount, evictionCount;
struct cacheLine
{
	int valitBit;
	int tag;
}



int main(int argc, char **argv)
{
	// parse the command line
	int opt, verboseFlag;
	int S, E, s, b;
	char filename[20];
	verboseFlag = 0;
	while((opt = getopt(argc, argv, "vs:E:b:t:")) != -1)
	{
		switch (opt)
		{
			case 'v':
				verboseFlag = 1;
				break;
			case 's':
				s = atoi(optarg);
				S = 2 ^ s;
			case 'E':
				E = atoi(optarg);
			case 'b':
				b = atoi(optarg);
			case 't':
				filename = optarg;
			default: /* ? */
				fprintf(stderr, "./csim\
						[-hv] -s <s> -E <E> -b <b> -t <tracefile>");
				exit(EXIT_FAILURE);
		}
	}

	if(optind >= argc)
	{
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}

	printf("name argument = %s\n", argv[optind]);

	// parse the memory access instructions and do caching
	FILE *traceFile;
	traceFile = fopen(filename, "r");
	
	char identifier;
	unsigned address;
	int size;

	while(fscanf(traceFile, " %c %x,%d", &identifier, &address, &size) > 0)
	{
		caching()
	}

	fclose(traceFile);
    printSummary(0, 0, 0);
	exit(EXIT_SUCCESS);
}

int caching(char identifier, unsigned address, int size)
{
	// parse the address
	
}
