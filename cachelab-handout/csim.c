/* author: MCX;		ID: hinmo */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cachelab.h"

// define different return result 
#define HIT 1
#define MISS 2
#define MISS_AND_EVICTION 3
#define MISS_AND_HIT 4
#define MISS_EVICTION_AND_HIT 5
#define HIT_HIT 6

// global variables 
int hitCount, missCount, evictionCount;
int S, E;
extern  char *optarg;
extern int optind, opterr, optopt;
struct cacheLine
{
	int validBit;
	unsigned tag;
	int accessTime;
};

typedef struct cacheLine line; 

// function prototype
int getopt(int argc, char * const argv[], const char *optstring);
int initCaches(line * caches, int S, int E);
int caching(line *caches, char identifier, 
		unsigned long address, int size, int b, int s);
void updateAccessTime(line *caches, int setInd, int lineNo);
// void printVerbose()
// void cmdParsor()

int main(int argc, char **argv)
{
	hitCount = 0;
	missCount = 0;
	evictionCount = 0;
	// parse the command line
	int opt, verboseFlag;
	int s, b;
	int result;
	char traceName[100];
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
				S = pow(2, s);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 't':
				strcpy(traceName,optarg);
				break;
			default: /* ? */
				fprintf(stderr, "./csim\
						[-hv] -s <s> -E <E> -b <b> -t <tracefile>");
				exit(EXIT_FAILURE);
		}
	}

// 	if(optind >= argc)
// 	{
// 		fprintf(stderr, "Expected argument after options\n");
// 		exit(EXIT_FAILURE);
// 	}

	// printf("name argument = %s\n", argv[optind]);

	// init the cache;
	int isInitSuccess;
	line *caches;
	caches = (line *) malloc(S*E*sizeof(line));
	if (!caches) {
		fprintf(stderr, "Memory error!\n");
		exit(-1);
	}
	isInitSuccess = initCaches(caches, S, E);
	// initCaches(caches, S, E);
	if(!isInitSuccess)
	{
	printf("cache initialization failure.");
	}

	// parse the memory access instructions and do caching
	FILE *traceFile;
	traceFile = fopen(traceName, "r");
	
	char identifier;
	unsigned long address;
	int size;
	int cshCount=0;

	while(fscanf(traceFile, " %c %lx,%d", &identifier, &address, &size) > 0)
	{
		if(identifier != 'I')
		{
			cshCount++;
			result = caching(caches, identifier, address, size, b, s);
			if(verboseFlag == 1)
			{ 
				switch(result)
				{
					case HIT_HIT:
						printf("%c %lx,%d hit hit\n", identifier, address, size);
						break;
					case HIT:
						printf("%c %lx,%d hit\n", identifier, address, size);
						break;
					case MISS:
						printf("%c %lx,%d miss\n", identifier, address, size);
						break;
					case MISS_AND_HIT:
						printf("%c %lx,%d miss hit\n", identifier, address, size);
						break;
					case MISS_AND_EVICTION:
						printf("%c %lx,%d miss eviction\n",identifier, address, size);
						break;
					case MISS_EVICTION_AND_HIT:
						printf("%c %lx,%d miss eviction hit\n",
								identifier, address, size);
						break;
				}
			}
		}
	}
	// end
	free(caches);
	fclose(traceFile);
    printSummary(hitCount, missCount, evictionCount);
	exit(EXIT_SUCCESS);
}

int initCaches(line * caches, int S, int E)
{
	if(caches != NULL)
	{
		for(int i=0; i < (S*E); i++)
		{
			caches[i].validBit = 0;
			caches[i].tag = 0;
			caches[i].accessTime = 0;
		}
		return 1;
	}
	else 
		return 0;
}

	// every opt need 2 loops in the worst case, and the same code.
int caching(line *caches, char identifier, unsigned long address, int size, int b, int s)
{
	// parse the address
	unsigned tag, setInd, setIndMask;
	setIndMask = 1;
	for(int i = 0; i < (s-1); i++)
	{
		setIndMask = (setIndMask << 1) + 1;
	}
	setIndMask = setIndMask << b;
	setInd =( (address & setIndMask) >> b);
	tag = (address ) >> (b + s);
	// for search, bst would be good, but now not needed.
	// L: hit; miss; miss_& eviction;	
	// S: hit; miss; miss & eviction;	
	// M: hit & hit; miss & hit; miss & eviction & hit;
	for(int i = 0; i < E; i++)
	{
		// hit
		if(caches[setInd*E + i].validBit == 1 
				&& (caches[setInd*E + i].tag ^ tag) == 0 )
		{
			updateAccessTime(caches, setInd, i);
			if(identifier == 'M')
			{
				hitCount = hitCount + 2;	// load hit and store hit
				return HIT_HIT;
			}
			else 
			{
				hitCount++;
				return HIT;
			}
		}
	}
	// Miss and meet a empty line
	for(int i = 0; i < E; i++)
	{
		if(caches[setInd*E + i].validBit == 0 )
		{
			caches[setInd*E + i].validBit = 1;
			caches[setInd*E + i].tag = tag;
			missCount++;
			updateAccessTime(caches, setInd, i);
			if(identifier == 'M')
			{
				hitCount++;	// store hit
				return MISS_AND_HIT;
			}
			else 
			{
				return MISS;
			}
		}
	}
	// Miss and need eviction
	int lruLine = 0; 
	for(int i = 0; i < E; i++)
	{
		if(1 ==  caches[setInd*E + i].accessTime)
		{
			lruLine = i;
		}
	}
	(*(caches + setInd*E + lruLine)).validBit = 1;
	(*(caches + setInd*E + lruLine)).tag = tag;
	missCount++;
	evictionCount++;
	updateAccessTime(caches, setInd, lruLine);
	if(identifier == 'M')
	{
		hitCount++;
		return MISS_EVICTION_AND_HIT;
	}
	return MISS_AND_EVICTION;
}


void updateAccessTime(line *caches, int setInd, int lineNo)
{
	for(int i = 0; i < E; i++)
	{
		if(caches[setInd*E + i].validBit == 1 &&
					caches[setInd*E + i].accessTime >
					 (*(caches + setInd*E + lineNo)).accessTime)
		{
			--caches[setInd*E + i].accessTime;
		}
	}
	(*(caches + setInd*E + lineNo)).accessTime = E;
}
