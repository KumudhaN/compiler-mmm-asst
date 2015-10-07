#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

#define M 4096
#define N 4096
#define K 4096
#define alpha 1
#define beta 1
#define TILE 512
#define MIN(a,b) (((a)<(b))?(a):(b))
double A[M][K];
double B[K][N];
double C[M][N];

#ifdef TIME
#define IF_TIME(foo) foo;
#else
#define IF_TIME(foo)
#endif

void init_array()
{
    int i, j;

    #pragma omp parallel for private(i,j)
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            A[i][j] = (i + j);
            B[i][j] = (double)(i*j);
            C[i][j] = 0.0;
        }
    }
}


void print_array()
{
    int i, j;

    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            fprintf(stdout, "%lf ", C[i][j]);
            if (j%80 == 79) fprintf(stdout, "\n");
        }
        fprintf(stdout, "\n");
    }
}

double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

double t_start, t_end;

int main()
{
    int i, j, k, ii, jj, kk;
    register double s;

    init_array();

    IF_TIME(t_start = rtclock());

    /* Code to be optimized - start */
    #pragma omp parallel for private(i,j,k,ii,jj,kk)   
    for(i=0; i<M; i+=TILE)		
        for(k=0; k<K; k+=TILE)
	       for(j=0; j<N; j+=TILE)  
				for(ii=i; ii<MIN(M,i+TILE); ii++)		
					for(kk=k; kk<MIN(K,k+TILE); kk++)
						for(jj=j; jj<MIN(N,j+TILE); jj++)  
							C[ii][jj] = beta*C[ii][jj] + alpha*A[ii][kk] * B[kk][jj];
    /* Code to be optimized - end */

    IF_TIME(t_end = rtclock());
    IF_TIME(fprintf(stderr, "%0.6lfs\n", t_end - t_start));

    if (fopen(".test", "r")) {
        print_array();
    }

    return 0;
}
