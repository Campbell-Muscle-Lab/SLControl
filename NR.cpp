// NR functions below this point

#include <stdafx.h>
#include <math.h>


#include "NR.h"

int ndatat;
float *xt,*yt,aa,abdevt;

void nrerror(char error_text[])
{
	// Print error text to file

	FILE *error_file=fopen("c:\\temp\\nr_error.txt","w");

	fprintf(error_file,"%s\n",error_text);

	fclose(error_file);
}

int *ivector(long nl, long nh)
{
	// Allocate an integer vector

	int *v;

	v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl+NR_END;
}

void free_ivector(int *v, long nl, long nh)
{
	// Free an integer vector

	free((FREE_ARG) (v+nl-NR_END));
}


float *vector(long nl, long nh)
{
	// Allocate a floating point vector

	float *v;

	v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));

	if (!v) nrerror("Allocation failure in vector()");
	return v-nl+NR_END;
}

void free_vector(float *v, long nl, long nh)
{
	// Frees a vector

	free((FREE_ARG) (v+nl-NR_END));
}

float **matrix(long nrl, long nrh, long ncl, long nch)
{
	// Allocate memory for a matrix

	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	float **m;

	// allocate pointers to rows
	m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	// allocate rows and set pointers to them
	m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
	if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

	// return pointer to array of pointers to rows
	return m;
}

void free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
{
	// Frees memory for a matrix

	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}

float select(unsigned long k, unsigned long n, float arr[])
{
	// Returns the kth smallest value in the array arr[]

	unsigned long i,ir,j,l,mid;
	float a,temp;

	l=1;
	ir=n;
	for (;;) {
		if (ir <= l+1) {
			if (ir == l+1 && arr[ir] < arr[l]) {
				SWAP(arr[l],arr[ir])
			}
			return arr[k];
		} else {
			mid=(l+ir) >> 1;
			SWAP(arr[mid],arr[l+1])
			if (arr[l+1] > arr[ir]) {
				SWAP(arr[l+1],arr[ir])
			}
			if (arr[l] > arr[ir]) {
				SWAP(arr[l],arr[ir])
			}
			if (arr[l+1] > arr[l]) {
				SWAP(arr[l+1],arr[l])
			}
			i=l+1;
			j=ir;
			a=arr[l];
			for (;;) {
				do i++; while (arr[i] < a);
				do j--; while (arr[j] > a);
				if (j < i) break;
				SWAP(arr[i],arr[j])
			}
			arr[l]=arr[j];
			arr[j]=a;
			if (j >= k) ir=j-1;
			if (j <= k) l=i;
		}
	}
}

void medfit(float x[], float y[], int ndata, float *a, float *b, float *abdev)
{
	// Fits y=a+bx by the criterion of least absolute deviations

//	float rofunc(float b);
	int j;
	float bb,b1,b2,del,f,f1,f2,sigb,temp;
	float sx=0.0,sy=0.0,sxy=0.0,sxx=0.0,chisq=0.0;

	ndatat=ndata;
	xt=x;
	yt=y;
	for (j=1;j<=ndata;j++) {
		sx += x[j];
		sy += y[j];
		sxy += x[j]*y[j];
		sxx += x[j]*x[j];
	}
	del=ndata*sxx-sx*sx;
	aa=(sxx*sy-sx*sxy)/del;
	bb=(ndata*sxy-sx*sy)/del;
	for (j=1;j<=ndata;j++)
		chisq += (temp=y[j]-(aa+bb*x[j]),temp*temp);
	sigb=(float)sqrt(chisq/del);
	b1=bb;
	f1=rofunc(b1);
	b2=bb+(float)SIGN((float)3.0*sigb,f1);
	f2=rofunc(b2);
	while (f1*f2 > 0.0) {
		bb=(float)2.0*b2-b1;
		b1=b2;
		f1=f2;
		b2=bb;
		f2=rofunc(b2);
	}
	sigb=(float)0.01*sigb;
	while (fabs(b2-b1) > sigb) {
		bb=(float)0.5*(b1+b2);
		if (bb == b1 || bb == b2) break;
		f=rofunc(bb);
		if (f*f1 >= 0.0) {
			f1=f;
			b1=bb;
		} else {
			f2=f;
			b2=bb;
		}
	}
	*a=aa;
	*b=bb;
	*abdev=abdevt/ndata;
}

#define EPS 1.0E-7
float rofunc(float b)
{
	float select(unsigned long k, unsigned long n, float arr[]);
	int j;
	float *arr,d,sum=0.0;

	arr=vector(1,ndatat);
	for (j=1;j<=ndatat;j++) arr[j]=yt[j]-b*xt[j];
	if (ndatat & 1) {
		aa=select((ndatat+1)>>1,ndatat,arr);
	}
	else {
		j=ndatat >> 1;
		aa=(float)0.5*(select(j,ndatat,arr)+select(j+1,ndatat,arr));
	}
	abdevt=0.0;
	for (j=1;j<=ndatat;j++) {
		d=yt[j]-(b*xt[j]+aa);
		abdevt += (float)fabs(d);
		if (yt[j] != 0.0) d /= (float)fabs(yt[j]);
		if (fabs(d) > EPS) sum += (d >= 0.0 ? xt[j] : -xt[j]);
	}
	free_vector(arr,1,ndatat);
	return sum;
}
#undef EPS

void mrqmin(float x[], float y[], float sig[], int ndata, float a[], int ia[],
	int ma, float **covar, float **alpha, float *chisq,
	void (*funcs)(float, float [], float *, float [], int), float *alamda)
{
	void covsrt(float **covar, int ma, int ia[], int mfit);
	void gaussj(float **a, int n, float **b, int m);
	void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
		int ia[], int ma, float **alpha, float beta[], float *chisq,
		void (*funcs)(float, float [], float *, float [], int));
	int j,k,l,m;
	static int mfit;
	static float ochisq,*atry,*beta,*da,**oneda;

	if (*alamda < 0.0) {
		atry=vector(1,ma);
		beta=vector(1,ma);
		da=vector(1,ma);
		for (mfit=0,j=1;j<=ma;j++)
			if (ia[j]) mfit++;
		oneda=matrix(1,mfit,1,1);
		*alamda=(float)0.001;
		mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq,funcs);

		ochisq=(*chisq);
		for (j=1;j<=ma;j++) atry[j]=a[j];
	}

	for (j=0,l=1;l<=ma;l++) {
		if (ia[l]) {
			for (j++,k=0,m=1;m<=ma;m++) {
				if (ia[m]) {
					k++;
					covar[j][k]=alpha[j][k];
				}
			}
			covar[j][j]=alpha[j][j]*((float)1.0+(*alamda));
			oneda[j][1]=beta[j];
		}
	}
	gaussj(covar,mfit,oneda,1);
	for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
	if (*alamda == 0.0) {
		covsrt(covar,ma,ia,mfit);
		free_matrix(oneda,1,mfit,1,1);
		free_vector(da,1,ma);
		free_vector(beta,1,ma);
		free_vector(atry,1,ma);
		return;
	}
	for (j=0,l=1;l<=ma;l++)
		if (ia[l]) atry[l]=a[l]+da[++j];
	mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq,funcs);
	if (*chisq < ochisq) {
		*alamda *= (float)0.1;
		ochisq=(*chisq);
		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				for (j++,k=0,m=1;m<=ma;m++) {
					if (ia[m]) {
						k++;
						alpha[j][k]=covar[j][k];
					}
				}
				beta[j]=da[j];
				a[l]=atry[l];
			}
		}
	} else {
		*alamda *= 10.0;
		*chisq=ochisq;
	}
}

void mrqcof(float x[], float y[], float sig[], int ndata, float a[], int ia[],
	int ma, float **alpha, float beta[], float *chisq,
	void (*funcs)(float, float [], float *, float [], int))
{
	int i,j,k,l,m,mfit=0;
	float ymod,wt,sig2i,dy,*dyda;

	dyda=vector(1,ma);
	for (j=1;j<=ma;j++)
		if (ia[j]) mfit++;
	for (j=1;j<=mfit;j++) {
		for (k=1;k<=j;k++) alpha[j][k]=0.0;
		beta[j]=0.0;
	}
	*chisq=0.0;

	for (i=1;i<=ndata;i++) {
		(*funcs)(x[i],a,&ymod,dyda,ma);
		sig2i=(float)1.0/(sig[i]*sig[i]);
		dy=y[i]-ymod;
		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				wt=dyda[l]*sig2i;
				for (j++,k=0,m=1;m<=l;m++)
					if (ia[m]) alpha[j][++k] += wt*dyda[m];
				beta[j] += dy*wt;
			}
		}
		*chisq += dy*dy*sig2i;
	}

	for (j=2;j<=mfit;j++)
		for (k=1;k<j;k++) alpha[k][j]=alpha[j][k];
	free_vector(dyda,1,ma);
}

void covsrt(float **covar, int ma, int ia[], int mfit)
{
	int i,j,k;
	float temp;

	for (i=mfit+1;i<=ma;i++)
		for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
	k=mfit;
	for (j=ma;j>=1;j--) {
		if (ia[j]) {
			for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j])
			for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
			k--;
		}
	}
}


void gaussj(float **a, int n, float **b, int m)
{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,temp;

	indxc=ivector(1,n);
	indxr=ivector(1,n);
	ipiv=ivector(1,n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=(float)fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1) nrerror("gaussj: Singular Matrix-1");
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
			for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix-2");
		pivinv=(float)1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
	}
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
}

void fexpfit(float x, float a[], float *y, float dyda[], int na)
{
	*y=a[1]*(float)(1.0-exp(-a[2]*x));

	dyda[1]=(float)(1.0-exp(-a[2]*x));
	dyda[2]=x*a[1]*(float)exp(-a[2]*x);
}

void fdoubleexpfit(float x, float a[], float *y, float dyda[], int na)
{
	*y=(float)((a[1]*(1.0-exp(-a[2]*x)))+(a[3]*(1.0-exp(-a[4]*x))));

	dyda[1]=(float)(1.0-exp(-a[2]*x));
	dyda[2]=x*a[1]*(float)exp(-a[2]*x);
	dyda[3]=(float)(1.0-exp(-a[4]*x));
	dyda[4]=x*a[3]*(float)exp(-a[4]*x);
}
