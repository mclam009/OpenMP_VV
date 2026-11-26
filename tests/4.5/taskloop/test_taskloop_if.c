//--------------------------------------------- test_taskloop_if.c ----------------------------------------//
//
// OpenMP API Version 4.5 November 2015
//
// This test checks the taskloop directive with the if clause specified.
// The 'taskloop' construct parallelize loops with independent iterations by creating tasks. 
// It allows for efficient parallel execution of loop iterations by distributing them among multiple threads. 
// The 'if' clause used to check the condition and taskloop will take into consideration only,      
// if the condition is TRUE.
//----------------------------------------------------------------------------------------------------------//

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "ompvv.h"

#define NUM_TASKS 1000
#define NUM_THREADS 1000

#define M 100

//if all thread ids are same, return 1 else return 0
int thread_ids_are_same(int a[], int len) {

	int first = a[0];

	for(int i = 1; i < len; i++) {
		if(first != a[i]) 
		  return 0; //if all the thread ids are not same
	}

	return 1; //if all the thread ids are same	
}

int test_taskloop_if(int THRESHOLD) {
  
  int errors = 0;
  
  int thread_ids[NUM_TASKS];
  // pstl-omp: with if(true) the tasks are deferrable, and the specification lets one thread of the team, the
  // encountering one or another, run all of them, so that case checks only that every iteration ran, and reports one
  // thread as a warning. With if(false) the tasks are undeferred, and the test still checks that one thread ran them.
  // The distinct negative initial values make that check fail, too, if an iteration did not run.
  int num_threads = 0, all_ran = 1;
  for (int i = 0; i < NUM_TASKS; i++)
    thread_ids[i] = -1 - i;

  #pragma omp parallel num_threads(NUM_THREADS)
  {
  	#pragma omp single
    	{	
        	num_threads = omp_get_num_threads();
        	#pragma omp taskloop if(M == THRESHOLD)
      		for (int i = 0; i < NUM_TASKS; i++) {
	 		thread_ids[i] = omp_get_thread_num();
      		} 
   	}
  } 
  
  if (M ==  THRESHOLD) {
    OMPVV_WARNING_IF(num_threads > 1 && thread_ids_are_same(thread_ids, NUM_TASKS),
                     "One of %d threads ran all %d iterations of the deferrable tasks", num_threads, NUM_TASKS);
    for (int i = 0; i < NUM_TASKS; i++) {
      if (thread_ids[i] < 0 || thread_ids[i] >= num_threads)
        all_ran = 0;
    }
    if (!all_ran) {
      errors++; 
    }
  } else if (M != THRESHOLD) {
    if (thread_ids_are_same(thread_ids, NUM_TASKS) == 0) {
      errors++; 
    }
  }

  return errors;            

}

int main() {
 
   int errors = 0;
   
   int THRESHOLD = 100; //Equal to M
   OMPVV_TEST_AND_SET_VERBOSE(errors, (test_taskloop_if(THRESHOLD) != 0));

   THRESHOLD = 110; // Not equal to M
   OMPVV_TEST_AND_SET_VERBOSE(errors, (test_taskloop_if(THRESHOLD) != 0));
   
   OMPVV_REPORT_AND_RETURN(errors);

   return 0;
}
