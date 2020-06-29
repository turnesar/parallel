#include <xmmintrin.h>
#include <omp.h>
#include <stdio.h>

#define SSE_WIDTH		4

#define SIMDFLAG	0

#define NUMTRIALS      10
// setting the number of items in the array
#ifndef NUMNODES
#define NUMNODES	1024 //1024 to 8*1024*1024
#endif

// setting the flag to run SIMD or not
#ifndef NUMT
#define NUMT	1
#endif

//global variables
float arrayA[NUMNODES] = { 0. }; 
float arrayB[NUMNODES] = { 0. }; 

//function prototypes
float SimdMulSum( float *a, float *b, int len);
float teamWork(int me);
float MulSum( float *a, float *b, int len );

//main program
int main( int argc, char *argv[ ] ){

	#ifndef _OPENMP
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif


  
	omp_set_num_threads( NUMT );
	//fill our arrays 
	for( int i = 0; i < NUMNODES; i++ )
	{
		float x = 2.; 
		arrayA[i] = x;
		arrayB[i] = x - 0.8;
    }
	
	printf("NUMNODES: %d\n", NUMNODES);
    printf("SIMD? %d\n", SIMDFLAG);

	for(int n=0; n < NUMTRIALS; n++){
	   	float total = 0;
        double time0 = omp_get_wtime( );
        //spmd approach and use thread number to decide what range of array
        //indeces to give to simd.
        #pragma omp parallel default(none) reduction(+:total) 
        {
            int me = omp_get_thread_num();
            total = teamWork(me);         
        }
    	double time1 = omp_get_wtime( );
    	//elapsed time in seconds, number of nodes in the array 
		printf( "%f\n", time1-time0);
	}
    return 0;
}

//teamwork makes the dream work, multithread for simd
float
teamWork(int me)
{
    int split = NUMNODES/NUMT; 
    int first = me * split;
    if(SIMDFLAG){
        return SimdMulSum(&arrayA[first], &arrayB[first], split);
    }
    else{
        return MulSum(&arrayA[first], &arrayB[first], split);
    }    
}

//Simd from professor [need to change for range of an array]
float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
  	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] );
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}

//without SIMD function
float
MulSum( float *a, float *b, int len )
{
   //mulsum without simd
    float sum =0; 
   	for( int i = 0; i < len; i++ )
	{
        sum += a[i] * b[i];
	}
    return sum;// return float 
}