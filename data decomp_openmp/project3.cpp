#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>

// setting the number of threads: 3 to test then 4
#ifndef NUMT
#define NUMT		4
#endif

//global constants
const float GRAIN_GROWS_PER_MONTH =	9.0; 	//inches 
const float ONE_DEER_EATS_PER_MONTH = 1.0;  //inches
const float AVG_PRECIP_PER_MONTH = 7.0;		// average inches
const float AMP_PRECIP_PER_MONTH = 6.0;		// plus or minus
const float RANDOM_PRECIP =	2.0;			// plus or minus noise
const float AVG_TEMP = 60.0;				// average fahrenheit
const float AMP_TEMP = 20.0;				// plus or minus
const float RANDOM_TEMP = 10.0;				// plus or minus noise
const float MIDTEMP = 40.0;					
const float MIDPRECIP =	10.0;

//global variables
int	NowYear = 2020;			// 2020 - 2025 years 
int	NowMonth = 0;			// 0 - 11 months
float NowPrecip = 0.;		// inches of rain per month
float NowTemp = 0.;			// temperature this month
float NowHeight = 1.;		// grain height in inches
int	NowNumDeer = 1;			// number of deer in the current population
int NowHunter = 2;          //my variable
unsigned int seed =0;

//function prototypes 
void setPrecipTemp();
float SQR (float x); 		
float Ranf(unsigned int *seedp,  float low, float high);  
int Ranf(unsigned int *seedp, int ilow, int ihigh);
void Watcher();
void GrainDeer();
void Grain();
void Hunters();

/************************************
*main program 
*************************************/
int main( int argc, char *argv[ ] )
{
	#ifndef _OPENMP
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif

	//set our precip and temp for Now
 	setPrecipTemp();
	//print our start values
	printf("Month\tYear\tTemp\tPrecip\tHunter\tHeight\tNumDeer\n");
	//printf( "%d\t%d\t%8.2lf\t%8.2lf\t%d\t%8.2lf\t%d\n", NowMonth, NowYear, NowTemp, NowPrecip,NowHunter, NowHeight, NowNumDeer);
    
	//lets get threading 
	omp_set_num_threads( NUMT );	
	#pragma omp parallel sections
	{
		//graindeer involves what the deer are doing 
		#pragma omp section
		{
			GrainDeer();
		}
		//grain involves what the grass is doing 
		#pragma omp section
		{
			Grain();
		}
		//watcher increments values and prints 
		#pragma omp section
		{
			Watcher();
		}
		//hunting season numbers
		#pragma omp section
		{
			Hunters();
		}
	} 

	return 0;
}

/******************************
Grain calculates grass growth, part of thread function 
******************************/
void
GrainDeer()
{	
	while( NowYear < 2026 )
		{
				int NextNumDeer = NowNumDeer;
				if(NowNumDeer < (int)NowHeight){
					NextNumDeer++; 
				}
				else{
					NextNumDeer = NextNumDeer-1;
				}
				NextNumDeer = NextNumDeer - NowHunter;
				if(NextNumDeer<0){
					NextNumDeer = 0;
				}
			// DoneComputing barrier:
			#pragma omp barrier
				NowNumDeer = NextNumDeer;
			// DoneAssigning barrier:
			#pragma omp barrier 
			// DonePrinting barrier:
			#pragma omp barrier
		}
}

/******************************
Grain calculates grass growth, part of thread function 
******************************/
void
Grain()
{
	while( NowYear < 2026 )
		{
		    //how much the grain grows
				float NextHeight = NowHeight; 
				float tempFactor = exp( -SQR((NowTemp - MIDTEMP)/10.));
				float precipFactor = exp( -SQR((NowPrecip - MIDPRECIP)/10.));
				NextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
				NextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
				if(NextHeight < 0){
					NextHeight = 0.;
				}
			// DoneComputing barrier:
			#pragma omp barrier
				NowHeight = NextHeight;
			// DoneAssigning barrier:
			#pragma omp barrier
			// DonePrinting barrier:
			#pragma omp barrier
		}
}
/******************************
Watcher function to calculate new values, part of thread
******************************/
void
Watcher()
{
	while( NowYear < 2026 )
		{
			// DoneComputing barrier:
			#pragma omp barrier
			// DoneAssigning barrier:
			#pragma omp barrier
			    printf( "%d\t%d\t%8.2lf\t%8.2lf\t%d\t%8.2lf\t%d\n", NowMonth, NowYear, NowTemp, NowPrecip,NowHunter, NowHeight, NowNumDeer);
				if(NowMonth == 11){
					NowMonth = 0;
					NowYear++;
				}
				else{
					NowMonth++;
				}
				setPrecipTemp();
			// DonePrinting barrier:
			#pragma omp barrier
		}
}

/************************
during hunting season, hunters are in the field
**********************/
void
Hunters()
{
		while( NowYear < 2026 )
		{
		    //chemical increases based on precip 
			int NextHunter = NowHunter;
			//archers only
			if(NowMonth == 7 || NowMonth == 8){ 
				NextHunter = 1;
			}
			//guns
			else if((NowMonth >8 && NowMonth <=11) || NowMonth==0){
				NextHunter = 2;
			}
			else{

				NextHunter = 0; 
			}
			//Done Computing Barrier:
			#pragma omp barrier
			    NowHunter = NextHunter; 
			//Done Assigning Barrier:	
			#pragma omp barrier
			//Done Printing Barrier:
			#pragma omp barrier
		}		
}
/*************************************
/temp and precip for the month
**************************************/
void 
setPrecipTemp() 
{
	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP );
	//rintf("this is NowTemp, %8.2lf\n", NowTemp); 
	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf(&seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
		NowPrecip = 0.;
	//printf("this is NowPrecip, %8.2lf\n", NowPrecip); 
	
}	

/*************************************
/helper square function
**************************************/
float
SQR (float x)
{
	return x*x;
}	

/*************************************
/random number generation
**************************************/
float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r(seedp);   
		//printf("seed: %8.2lf ", r);           // 0 - RAND_MAX

       float temp =  ( low  +  r * ( high - low ) / (float)RAND_MAX   );
	   //printf("random float: %8.2lf", temp);
	   return temp;
}


/*************************************
/random float generation
**************************************/
int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

/*************************************
/time of day seed borrowed from project1
**************************************/
void
TimeOfDaySeed( )
{
	struct tm y2k = { 0 };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  timer;
	time( &timer );
	double seconds = difftime( timer, mktime(&y2k) );
	unsigned int seed = (unsigned int)( 1000.*seconds );    // milliseconds
	srand( seed );
}







