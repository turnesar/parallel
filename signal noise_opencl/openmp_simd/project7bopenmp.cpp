#include <xmmintrin.h>
#include <omp.h>
#include <stdio.h>

#define SSE_WIDTH		4

// setting the flag to run SIMD or not
#ifndef SIMDFLAG
#define SIMDFLAG	0
#endif

//run openmp 1 thread.  Then multiple threads. Then with SIMD.
#ifndef NUMT
#define NUMT	1
#endif


//function prototypes
float SimdMulSum( float *a, float *b, int len);
void teamWork(int me, int sizeArray);
float MulSum( float *a, float *b, int len );

//main program
int main( int argc, char *argv[ ] ){

	#ifndef _OPENMP
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif


    //reading the file into the arrays
    FILE *fp = fopen( "signal.txt", "r" );
    if( fp == NULL )
    {
        fprintf( stderr, "Cannot open file 'signal.txt'\n" );
        exit( 1 );
    }
    int Size;
    fscanf( fp, "%d", &Size );
    float *A =     new float[ 2*Size ];
    float *Sums  = new float[ 1*Size ];
    
    
    for( int i = 0; i < Size; i++ )
    {
        fscanf( fp, "%f", &A[i] );
        A[i+Size] = A[i];		// duplicate the array
    }
    fclose( fp );


    
    omp_set_num_threads( NUMT );
    
	

    double time0 = omp_get_wtime( );
        //spmd approach and use thread number to decide what range of array
        //indeces to give to simd.
		//you removed the reduction for total here 
    #pragma omp parallel default(none) shared(A, Sums, Size) 
    {
        int me = omp_get_thread_num();
        int split = Size/NUMT; 
        int first = me * split;
        int last = first + (split-1);
        float *temp = new float[split];
        for( int shift=first; shift < last; shift++ )
        {
	        float sum = 0.;
            if(SIMDFLAG){
                sum = SimdMulSum( &A[0], &A[0+shift], Size );
            }
            else{
                sum = MulSum(&A[0], &A[0+shift], Size);      
            }
		
	       temp[shift-first] = sum;	// note the "fix #2" from false sharing if you are using OpenMP
        }

        #pragma omp critical
        for(int i = 0; i<split;i++){
            Sums[i+first] = temp[i];
        } 
      
     delete [] temp;            
    }
    	double time1 = omp_get_wtime( );
    	//elapsed time in seconds, number of nodes in the array 
		printf( "%10.3lf\t performance\n", ((double)Size * (double)Size)/(time1-time0)/1000000.);
    //remove comments to see sine wave data   
    // for( int i = 1; i < Size; i++ )
    //{
	//fprintf(stdout, "%d\t%10.3lf\n",i, Sums[i]);		// print the array
    //}
    delete [] A; 
    delete [] Sums;
	return 0;
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

