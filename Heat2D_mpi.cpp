#include <iostream>
#include <math.h>     
#include "Timer.h"
#include<stdlib.h>
#include <cstdlib>
#include <mpi.h>
#include <omp.h>
using namespace std;

double a = 1.0;  // heat speed
double dt = 1.0; // time quantum
double dd = 2.0; // change in system
//int thread =1; //initialize thread to 1 for openmp 


void init(double *matrix,int size,int p)
{
   int x=p,y=size,z=size;
   for(int i = 0; i < x; i++)
   {
      for(int j = 0; j < y; j++)
      {
          for(int k = 0; k < z; k++)
          {
              int res = (i*y*z) + (j*z) + k;
               matrix[res] = 0.0;
          }
      }
    }
}
//Function to calculate index of elements in 1Darray
int computeIndex(int p,int x,int y ,int size_y)
{
   int index =  x*size_y+y;
   return index;
}

void heat2d(int max_time,double* z,int p,int stripe,int size,double r,int t,int heat_time,int mpi_id,int mpi_size)
{
     // #pragma omp parallel for
	 int start;
	int end;
	 for ( int y = 0; y < size; y++ ) 
	  {
      //Replace the 3d array index into 1D array representation
      //z[p][0][y] = z[p][1][y]
	  if(mpi_id==0)
	  {
      int indexOneLeft = computeIndex(0,0,y,size); //(p*size*size)+(0*size)+y;
      int indexTwoLeft=computeIndex(0,1,y,size);//(p*size*size)+(1*size)+y;
      z[indexOneLeft] = z[indexTwoLeft];
	  }
      //z[p][size - 1][y] = z[p][size - 2][y];
	  if(mpi_id==mpi_size-1)
	  {
      int indexOneRight = computeIndex(0,stripe-1,y,size); //(p*size*size)+((size-1)*size)+y;
      int indexTwoRight = computeIndex(0,stripe-2,y,size);    //(p*size*size)+((size-2)*size)+y;
      z[indexOneRight] = z[indexTwoRight];
	  }
	 }
	
    // two upper and lower rows are identical
    for ( int x = 0; x < stripe; x++ ) {
  //    z[p][x][0] = z[p][x][1];

        int indexUpperRowLeft =  computeIndex(0,x,0,size);//(p*size*size)+(x*size)+0;
        int indexUpperRowRight = computeIndex(0,x,1,size);//(p*size*size)+(x*size)+1;
         z[indexUpperRowLeft] = z[indexUpperRowRight];

        int indexLowerRowLeft = computeIndex(0,x,size-1,size);// (p*size*size)+(x*size)+(size-1);
          int indexLowerRowRight = computeIndex(0,x,size-2,size); //(p*size*size)+(x*size)+(size-2);
         z[indexLowerRowLeft] = z[indexLowerRowRight];
        //z[p][x][size - 1] = z[p][x][size - 2];
    }

    //if(mpi_id==0)   //only master
	//{	
    // keep heating the bottom until t < heat_time
	
	start = mpi_id * stripe > size/3 ? 0 : size/3 - mpi_id*stripe;
	end = mpi_id * (stripe + 1) < 2*size/3 ? stripe : 2*size/3 - mpi_id*(stripe);
	if(start < end)
	{
		if ( t < heat_time ) 
		{
		 for ( int x =  start; x < end ; x++ )
		 z[x*size+0] = 19.0; // heat
        }
	}
 }
  void eulerMethod(double* z, double* zp0,double* zp1, int p,int stripe,int size,double r)
{
    // perform forward Euler method
    int p2 = (p + 1) % 2;
	double* zp = p2 == 0 ? zp0 : zp1;

	#pragma omp parallel for firstprivate (zp)  
    for ( int x = 1; x < stripe - 1; x++ )
	{
      for ( int y = 1; y < size - 1; y++ )
/*        z[p2][x][y] =
          z[p][x][y] +
          r * ( z[p][x + 1][y] - 2 * z[p][x][y] + z[p][x - 1][y] ) +
          r * ( z[p][x][y + 1] - 2 * z[p][x][y] + z[p][x][y - 1] ) ;
*/
          {   
          int pIndex = computeIndex(0,x,y,size);
          int rightElementIndex = computeIndex(0,x+1,y,size);
          int leftElementIndex = computeIndex(0,x-1,y,size);
          int upElementIndex = computeIndex(0,x,y+1,size);
          int downElementIndex = computeIndex(0,x,y-1,size);
		  
           zp[x*size+y] = z[pIndex] +
           r * (z[rightElementIndex]- 2 * z[pIndex]+ z[leftElementIndex]) +
           r * (z[upElementIndex] - 2 * z[pIndex]+ z[downElementIndex]);
          }
	  }
}
 // end of simulation



int main( int argc, char *argv[] ) {
  // verify arguments
  if ( argc != 6 ) {
    cerr << "usage: Heat2D size max_time heat_time interval" << endl;
    return -1;
  }
  int size = atoi( argv[1] );
  int max_time = atoi( argv[2] );
  int heat_time = atoi( argv[3] );
  int interval  = atoi( argv[4] );
  int thread = atoi (argv[5]);
  double r = a * dt / ( dd * dd );
  
  int offset; 
  int stripe;
  //double z[2][size][size];
  int mpi_id,mpi_size;
  int root_process;
  int p_id;
  MPI_Status status;
//  int MPI_Barrier(MPI_Comm comm);
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&mpi_id);
  MPI_Comm_size(MPI_COMM_WORLD,&mpi_size);

  root_process =0;
 
 //matrix initialization
  double *z = new double[2*size*size];
  //double *zp;
 //initialize an array z with 0.0 values initially

	init(z,size,2);
 //broacast the size of array to all the processor
   MPI_Bcast(&size,1,MPI_INT,0,MPI_COMM_WORLD);

  // start a timer
  Timer time;
  time.start( );
  omp_set_num_threads(thread);

 
    //Dividing the size of data among available processes
    stripe = size/mpi_size; 
	
	//If p==0 then the offset used in Euler function
    int offsetp0 = (0*size*size)+(size*stripe*mpi_id);
	//if p==1 then the offset used in Euler function
	int offsetp1 = (1*size*size)+(size*stripe*mpi_id);
    
  for ( int t = 0; t < max_time; t++ ) {
    int p = t % 2; // p = 0 or 1: indicates the phase
	offset = (p*size*size)+(size*stripe*mpi_id);
	
	//simulate heat diffusion
    heat2d(max_time,z+offset,p,stripe,size,r,t,heat_time,mpi_id,mpi_size);
	//To identify whether process is even or odd 
    int even = mpi_id%2;
	//To synchronize send and receive between previous process, process itself and neighbouring process 
	if(even == 0)  // even process
	{
		if(mpi_id > 0)
		{
		    //sending/receiving from N-1 process's first element
			MPI_Send(z+offset,size,MPI_DOUBLE,mpi_id-1,3,MPI_COMM_WORLD);
			MPI_Recv(z+offset,size,MPI_DOUBLE,mpi_id-1,2,MPI_COMM_WORLD,&status);
		}
		
		if( mpi_id < mpi_size - 1)
		{
		    //sending/receiving from N+1 process's last element
			//	MPI_Sendrecv(z1+stripe-1,1,MPI_DOUBLE,mpi_id+1,0,z1+stripe-1,1,MPI_DOUBLE,mpi_id,+10,MPI_COMM_WORLD,&status);	
			MPI_Send(z+offset+(stripe-1)*size,size,MPI_DOUBLE,mpi_id+1,2,MPI_COMM_WORLD);
			MPI_Recv(z+offset+(stripe-1)*size,size,MPI_DOUBLE,mpi_id+1,3,MPI_COMM_WORLD,&status);
		}
	}
	else // odd process
	{
		if(mpi_id > 0)
		{
		    //sending/receiving from N-1 process's first element
			MPI_Recv(z+offset,size,MPI_DOUBLE,mpi_id-1,2,MPI_COMM_WORLD,&status);
			MPI_Send(z+offset,size,MPI_DOUBLE,mpi_id-1,3,MPI_COMM_WORLD);
		}
		
		if( mpi_id < mpi_size - 1)
		{
		     //sending/receiving from N+1 process's last element
			//	MPI_Sendrecv(z1+stripe-1,1,MPI_DOUBLE,mpi_id+1,0,z1+stripe-1,1,MPI_DOUBLE,mpi_id,+10,MPI_COMM_WORLD,&status);	
			MPI_Recv(z+offset+(stripe-1)*size,size,MPI_DOUBLE,mpi_id+1,3,MPI_COMM_WORLD,&status);
			MPI_Send(z+offset+(stripe-1)*size,size,MPI_DOUBLE,mpi_id+1,2,MPI_COMM_WORLD);
		}
	}
	
    //  MPI_Barrier(comm); // Wait until all ranks completes task up till this point
    if(mpi_id!=0) // slave
    {
		MPI_Send(z+offset,size*stripe,MPI_DOUBLE,0,4,MPI_COMM_WORLD);
	}	
    else  // master
    {
		for(p_id=1;p_id<mpi_size;p_id++)
		{
			//Receives data from each process and print the intermediate result
			MPI_Recv(z+(p*size*size) + (size*stripe*p_id),size*stripe,MPI_DOUBLE,p_id,4,MPI_COMM_WORLD,&status);
			//MPI_Recv(z+size*stripe*p_id+size*size,size*stripe,MPI_DOUBLE,p_id,5,MPI_COMM_WORLD,&status);  
		}
	}	
    if(mpi_id==0)
    {	
	// display intermediate results
		if ( interval != 0 && ( t % interval == 0 || t == max_time - 1 ) ) {
			cout << "time = " << t << endl;
			for ( int y = 0; y < size; y++ ) {
				for ( int x = 0; x < size; x++ )
        //displayResultIndex = computeIndex(p,x,y,size);//(p*size*size)+(x*size)+y;
        //cout << floor (z[displayResultIndex]/2) << " " ;
					cout << floor( z[p*size*size+x*size+y] / 2 ) << " ";
				cout << endl;
			}
			cout << endl;
		}
		
	}
	 
	
	//MPI_Barrier(MPI_COMM_WORLD); // Wait until all ranks completes task up till this point
	
	eulerMethod(z+offset,z + offsetp0, z + offsetp1, p,stripe,size,r);
      
} 	// end of simulation

	// finish the timer
	if(mpi_id==0)
	{
		cerr << "Elapsed time = " << time.lap( ) << endl;
	}

	//  return 0;
	MPI_Finalize();
}
