#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>

// setting the number of threads: You must use at least 1, 2, and 4.
#ifndef NUMT
#define NUMT		1
#endif

// number of subdivisions, at least 8 different ones
#ifndef NUMNODES
#define NUMNODES	2
#endif

// ranges for X and Y 

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

//used for superquadric
#define N	4.

//function declaration
float Height( int, int );
//helper function for height
float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r < 0. )
	        return 0.;
	float height = pow( 1. - xn - yn, 1./(float)N );
	return height;
}

//main program 

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	omp_set_num_threads( NUMT );	// set the number of threads to use in the for-loop:`

	//Center the superquadric around (0,0,0)
	//xN + yN + zN = 1, where n=4
	
	double time0 = omp_get_wtime( );
	double volume=0; 
/*
In this code sample, NUMNODES is the number of nodes, or dots, subdividing the floor area.
 So, for example, NUMNODES=4 means that there are 4 dots on each side edge.
Each node has some amount of tile space surrounding it.
*/
float fullTileSize = (((XMAX - XMIN)/(float)(NUMNODES-1))*((YMAX-YMIN)/(float)(NUMNODES-1)));

#pragma omp parallel for default(none) shared(fullTileSize) reduction(+:volume)
for( int i = 0; i < NUMNODES*NUMNODES; i++ )
{
	int iu = i % NUMNODES;
	int iv = i / NUMNODES;
	float tileSize;
	float z = Height( iu, iv )*2.; //multiply the 0.-Height area you get by 2)	

	if((iu == 0 && iv == 0) || (iu == (NUMNODES-1) && iv<1) ||(iu==0 && iv==(NUMNODES-1)) || (iu==(NUMNODES-1) && iv>(NUMNODES-1))){  
		//this is for corners only / a quarter of the size
		tileSize = fullTileSize/4.;
	}
	else if(iu == 0|| iu == (NUMNODES-1) || iv<1 || iv>(NUMNODES-1)){
		//this is for edges / a half of the size
		tileSize = fullTileSize/2.;
	}
	else{
		tileSize = fullTileSize;
	}
	volume = volume+(tileSize*z);
}
//numnodes^2 / time is the performance MegaHeights Computed Per Second
		double time1 = omp_get_wtime( );
		double megaHeightsPerSecond = pow((double)NUMNODES,2.) / ( time1 - time0 ) / 1000000.;
		printf( "%d\t%d\t%8.2lf\t%8.2lf\n", NUMT, NUMNODES, volume, megaHeightsPerSecond);
		
return 0;
}

