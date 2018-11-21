// NR.h

#ifndef _NR_H_
#define _NR_H_

// Definitions

#define FREE_ARG char*
#define NR_END 1

#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
#define SQR(a) ((a)*(a))
#define SIGN(a,b) ((b) >=0.0 ? fabs(a) : -fabs(a))

void nrerror(char error_text[]);

int *ivector(long nl, long nh);
void free_ivector(int *v, long nl, long nh);

float *vector(long nl, long nh);
void free_vector(float *v, long nl, long nh);

float **matrix(long nrl, long nrh, long ncl, long nch);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);

float select(unsigned long k, unsigned long n, float arr[]);

void medfit(float x[], float y[], int ndata, float *a, float *b, float *abdev);

float rofunc(float b);

void mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ia[],
	int ma, float **covar, float **alpha, float *chisq,
	void (*funcs)(float, float [], float *, float [], int), float *alamda);

void fexpfit(float x, float a[], float *y, float dyda[], int na);
void fdoubleexpfit(float x, float a[], float *y, float dyda[], int na);

#endif