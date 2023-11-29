/*
 ============================================================================
 Name        : cs402_1.c
 Author      : Tim McNamara
 Version     :
 Copyright   : Your copyright notice
 Description : CS402 - Assignment 1
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define GRIDSIZE 50
// Count the number of neighbours a cell at position [i][j] has in grid array.
int neighbour_count(int i,int j, int array[GRIDSIZE][GRIDSIZE]){
		int count =0;
		int test_x = 0;
		int test_y = 8;

		for (int x = i-1; x<=i+1; x++)
		{
			if(i == test_x && j==test_y){printf("i = %d: ", x);}
			for (int y = j-1; y<=j+1; y++)
			{
				if ((y>=0 && x>=0)&& (y<GRIDSIZE && x<GRIDSIZE)&& !(x==i && y ==j))
				{	count+=array[x][y];
			}
		}
	}
	return count;
}
// Check whether a cell will survive or not based on it's number of neighbours
int will_it_live(int count, int alive)
{
	if (alive == 1)
	{
		if (count >= 2 && count < 4){return 1;}
		else{return 0;}
	}
	else
	{
		if (count == 3)	{return 1;}
		else{return 0;}
	}
	return 0;
}
//Take the current state as argument
int (*game_wrapper(int p8_game_state[GRIDSIZE][GRIDSIZE]))[GRIDSIZE]
{
	 int (*new_state)[GRIDSIZE] = malloc(sizeof(int[GRIDSIZE][GRIDSIZE]));
//Run for loop in parallel with threads set dynamically.
#pragma omp parallel for shared(p8_game_state) schedule(dynamic, 10)
	   for (int p=0; p < GRIDSIZE; p++){
		   for (int q=0; q<GRIDSIZE;q++){
	   		   new_state[p][q] = will_it_live(neighbour_count(p,q,p8_game_state), p8_game_state[p][q]);
	   		 }
 }
	   // return the new grid generated.
 return new_state;
}
int main (int argc, char *argv[]) {
  int original_state[GRIDSIZE][GRIDSIZE];
  int p8_game_state[GRIDSIZE][GRIDSIZE];
  int p1_game_state[GRIDSIZE][GRIDSIZE];
  int ITERATIONS = 10000;
  int r;
   //Seed the initial 50x50 grid
  for (int p=0; p < GRIDSIZE; p++){
	   for (int q =0; q < GRIDSIZE; q++){
   		   r=rand()%1000;
   		 if (q<GRIDSIZE){
   			original_state[p][q] = r>750 ? 1:0;
   			printf("%d", original_state[p][q]);
  		 }
   	   }
	   printf("\n");
   	 }



  // Store the original grid in work structure to preserve it for later test.
  for (int i=0; i<GRIDSIZE; i++){for (int j=0; j<GRIDSIZE; j++) {p8_game_state[i][j] = original_state[i][j];}}

  //set to use 8 threads
  omp_set_num_threads(8);

   // Run Game of life for N iterations (Defined by variable at start)

  // Initialize clock values for testing timing of parallel vs non parallel
   clock_t p8_start, p8_end, p1_start, p1_end;
   p8_start = clock();
   // Run for some num of iterations defined at entrance to main
   for (int runs=0; runs<ITERATIONS; runs++){
	   int (*n)[GRIDSIZE] = game_wrapper(p8_game_state);
	   for (int i=0; i<GRIDSIZE; i++)
	   {
		   for (int j=0; j<GRIDSIZE; j++)
		   {
			   p8_game_state[i][j] = n[i][j];
		   }
	   }
   free(n);
   }
   p8_end = clock();
   //seed the p1 game_state from our original_state
   for (int i=0; i<GRIDSIZE; i++){for (int j=0; j<GRIDSIZE; j++) {p1_game_state[i][j] = original_state[i][j];}}

   //set number of threads to run on to 1
   omp_set_num_threads(1);

   	// Start clock for one thread run
    p1_start = clock();
    //run for n iterations (defined at top of file)
    for (int runs=0; runs<ITERATIONS; runs++){
 	   //entry point to parallelised function
    	int (*n)[GRIDSIZE] = game_wrapper(p1_game_state);
 	   for (int i=0; i<GRIDSIZE; i++)
 	   {
 		   for (int j=0; j<GRIDSIZE; j++)
 		   {
 			  p1_game_state[i][j] = n[i][j];
 		   }
 	   }
    free(n);
    }
    p1_end = clock();

 printf("[\n");
// Check to ensure both threaded and multithreaded return the same grid at the end (Naive check for race conditions)
 bool identical_grids = 1;
 for (int i=0; i<GRIDSIZE; i++){
	 for (int j=0; j<GRIDSIZE; j++)
	 {
		 printf("%d", p8_game_state[i][j]);
		 if (p8_game_state[i][j]!=p1_game_state[i][j])
		 {
			 identical_grids = 0;
		 }
	 }
	 printf("\n");
 } printf("]");

 // Report Performance & Checks
printf("Are grids matching(1=True, 0 = False): %d\n", identical_grids);
printf("p8 Cycles taken: %f\n", ((double) p8_end-p8_start)/CLOCKS_PER_SEC);
printf("P1 Cycles taken: %f\n", ((double)p1_end-p1_start)/CLOCKS_PER_SEC);
printf("Performance Difference(SingleThreaded -MultiThreaded) in seconds: %f\n", ((double)p1_end-p1_start)/CLOCKS_PER_SEC - ((double)p8_end-p8_start)/CLOCKS_PER_SEC);
printf("Performance Difference(100/SingleThreaded*MultiThreaded) Percent of single thread time used to run multithread (Lower = better) : %f%%\n",
		100/(((double)p1_end-p1_start)/CLOCKS_PER_SEC)* (((double)p8_end-p8_start)/CLOCKS_PER_SEC));
printf("Performance Difference(100/MultiThreaded*SingleThreaded) in percent of singlethreaded relative to multi (Higher = better) : %f%%\n",
		100/(((double)p8_end-p8_start)/CLOCKS_PER_SEC)* (((double)p1_end-p1_start)/CLOCKS_PER_SEC));
 return 0;
}
