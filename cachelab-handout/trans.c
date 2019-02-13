/* author: MCX;		ID: hinmo */
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
// diagonal elements unconsidered
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	// bsize = sqrt(256/3)
	int i, j,  bsize, ii, jj, tmp, index;
	if(N == 32)
		bsize = 8;
	else if(N == 64)
		bsize = 4 ;
	else 
		bsize = 8;
	for(i = 0; i < N; i+=bsize)
		for(j = 0; j < M; j+=bsize)
			for(ii = i; (ii < i + bsize) && (ii < N); ii++)
			{
				for(jj = j; (jj < j + bsize) && (jj < M); jj++)
				{
					if(ii != jj)
					{
						B[jj][ii]  = A[ii][jj];
					}
					else
					{
						tmp = A[ii][jj];
						index = ii;
					}
				}
				if(i == j)
					B[index][index] = tmp;
			}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char transTrial2_desc[] = "Trial2 bise : 5";
void transTrial2(int M, int N, int A[N][M], int B[M][N])
{
	int i, j,  bsize, ii, jj, tmp;
	bsize = 5;
	for(i = 0; i < N; i+=bsize)
		for(j = 0; j < M; j+=bsize)
			for(ii = i; (ii < i + bsize) && (ii < N); ii++)
				for(jj = j; (jj < j + bsize) && (jj < M); jj++)
				{
					tmp = A[ii][jj];
					B[jj][ii] = tmp;
				}
}


char transTrial3_desc[] = "Trial3 bsize: 6";
void transTrial3(int M, int N, int A[N][M], int B[M][N])
{
	int i, j,  bsize, ii, jj, tmp;
	bsize = 6;
	for(i = 0; i < N; i+=bsize)
		for(j = 0; j < M; j+=bsize)
			for(ii = i; (ii < i + bsize) && (ii < N); ii++)
				for(jj = j; (jj < j + bsize) && (jj < M); jj++)
				{
					tmp = A[ii][jj];
					B[jj][ii] = tmp;
				}
}

// void transpose_submit(int M, int N, int A[N][M], int B[M][N])
// {
// 	int i, j,  bsize, ii, jj, tmp;
// 	if(M == 61)		bsize = 4;
// 	else bsize = 6;
// 	for(i = 0; i < N; i+=bsize)
// 		for(j = 0; j < M; j+=bsize)
// 		{
// 			for(ii = i; (ii < i + bsize) && (ii < N); ii++)
// 				for(jj = j; (jj < j + bsize) && (jj < M); jj++)
// 				{
// 					tmp = A[ii][jj];
// 					B[jj][ii] = tmp;
// 				}
// 			if(N == 32 && ii == 23) bsize = 4;
// 			if(N == 64 && ii == 59) bsize = 4;
// 		}
// }
/*
 *
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

    registerTransFunction(transTrial2, transTrial2_desc); 

    registerTransFunction(transTrial3, transTrial3_desc); 

    //  registerTransFunction(transTrial4, transTrial4_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

