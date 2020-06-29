#include <xmmintrin.h>
#include <omp.h>
#include <stdio.h>

#define SSE_WIDTH		4
#define NUMTRIALS      10
// setting the number of items in the array
#ifndef NUMNODES
#define NUMNODES	1024 //1024 to 8*1024*1024
#endif

// setting the flag to run SIMD or not
#ifndef SIMDFLAG
#define SIMDFLAG	0
#endif

// setting the flag to run SIMD or not
#ifndef NUMT
#define NUMT	1
#endif

//global variables
float arrayA[NUMNODES] = { 0. }; 
float arrayB[NUMNODES] = { 0. }; 

//function prototypes
float SimdMulSum( float *a, float *b, int len );
float MulSum( float *a, float *b, int len );

//main program
int main( int argc, char *argv[ ] ){

    float total = 0; 

	//fill our arrays 
	for( int i = 0; i < NUMNODES; i++ )
	{
		float x = 2.; 
		arrayA[i] = x;
		arrayB[i] = x - 0.8;
    }
	
	printf("NUMNODES: %d\n", NUMNODES);

	for(int n=0; n < NUMTRIALS; n++){
	   	double time0 = omp_get_wtime( );
    	if(SIMDFLAG){
        	total = SimdMulSum (arrayA, arrayB, NUMNODES);//call SIMD   	
		}
    	else{
        	total = MulSum(arrayA, arrayB, NUMNODES);//call non-SIMD
    	}
    	double time1 = omp_get_wtime( );
    	//elapsed time in seconds, number of nodes in the array 
		//printf( "%f\t%f\t%d\n", total, time1-time0, NUMNODES);
    	printf( "%f\n", time1-time0);
	}
    return 0;
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

//Simd from professor
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