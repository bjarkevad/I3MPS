/*
 ============================================================================
 Name        : testcache.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define M_SIZE 100;
//#define CLS 64;
//#define SM (CLS / sizeof (double))

int main(void) {
//	time_t start_t, stop_t;
	int time_start, time_stop;
	int i=0, j=0, k=0, i2=0, j2=0, k2=0;
	const int M_SIZE = 400;

	const int SM = 64 / sizeof (double);
	int N = M_SIZE;
	double res[M_SIZE][M_SIZE];
	double mul1[M_SIZE][M_SIZE], mul2[M_SIZE][M_SIZE];
	double tmp[M_SIZE][M_SIZE];
	double *rres, *rmul1, *rmul2;

#ifdef SLOW
	//	printf("slow\n");
	for (i = 0; i < N; ++i) {
	  for (j = 0; j < N; ++j) {
	    for (k = 0; k < N; ++k)
	      res[i][j] += mul1[i][k] * mul2[k][j];
	  }
	}
#endif

#ifdef FASTER
	//	printf("faster\n");
	for (i = 0; i < N; ++i) {
	  for (j = 0; j < N; ++j)
	    tmp[i][j] = mul2[j][i];
	}
	for (i = 0; i < N; ++i) {
	  for (j = 0; j < N; ++j) {
	    for (k = 0; k < N; ++k)
	      res[i][j] += mul1[i][k] * tmp[j][k];
	  }
	}
#endif

#ifdef FASTEST
	//	printf("fastest\n");

	for (i = 0; i < N; i += SM) {
	  for (j = 0; j < N; j += SM) {
	    for (k = 0; k < N; k += SM) {
	      for (i2 = 0, rres = &res[i][j], rmul1 = &mul1[i][k]; i2 < SM; ++i2, rres += N, rmul1 += N) {
		for (k2 = 0, rmul2 = &mul2[k][j]; k2 < SM; ++k2, rmul2 += N) {
		  for (j2 = 0; j2 < SM; ++j2)
		    rres[j2] += rmul1[k2] * rmul2[j2];
		}
	      }
	    }
	  }
	}
#endif


//	return EXIT_SUCCESS;
	return res[N][N];
}
