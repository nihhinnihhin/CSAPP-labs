data structure needed:

cache is just 2D array of cache lines:
	struct cacheLine cache[S][E];
	int S = 2 ^ s, is the number of sets
	int E is associativity

Each cacheLine has:
	bool valitBit;
	int tag;

Queue to keep track of the "LRU" cacheLine


main()
{
	init the cache;
	parse the command line and get value of 's', 'b', 'E', 't'(filename);
	fscanf() the trace file, store as an array: cachings = "L 0x123, 1";
	// Memory efficiency would be good if there was generator in C.

}

parseCmdLine()
{

}

initCache()
{

}

caching()
{
foreach c in cachings:
	strtok(), split the c, store as caching;// include<string.h>
	do the caching:
		parse the 64-bit(hex form) caching[ADDR] in to tag, setInd, offset;
		if (strcmp('caching[OPT]','L') == 0)	// load to cache from memory
			for line in cache[setInd]
				if (tag match && validBit == true)
					then hits++;
				else if(validBit == false)	// there is a empty line
				{
					misses++;
					set tag and validBit;
					MISS_ONLY
				}
				else
				{
					misses++;
					extract victim(i.e. tag) in lruQueue;
					eviction the victim;	// i.e. set tag and validBit
					MISS_AND EVICTION;
				}
			else if(Store)	// store to cache from cpu
			{
				almost same operation as Load;
			}
			// first load then store, two instructions, so store must hit.
			//use write-back policy;
			else if(Modify)	
			{
				first load, then store, same operation and policy as above
				if hit
						hits++;
				if miss
					misses++;

			}

}

