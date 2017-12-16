/****************************************************************************
* pthread_pi_estimation.c
*
* 
* gcc -g -Wall -o pthread_pi_estimation_increment pthread_pi_estimation_increment.c -lm -lpthread
* Trong Minh Vo, 28 Nov 2017
*
* 
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


long long int g_sumTossInCircle, g_numberOfTossesSoFar;
pthread_mutex_t mutex;

double rand01();
void* do_Tossing_Serial (void* totalToss);
void* do_Tossing_Update_each_step (void* rankToss);
void* do_Tossing_Update_at_End (void* rankToss);

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

int main (int argc, char *argv[]){
  
  double start, finish;
  long long int totalToss, rankToss;
  int thread_count;
  long       thread;  /* Use long in case of a 64-bit system */
  pthread_t* thread_handles;

  int thread_increment;
  double* A_ExeTime_eachstep;
  double* A_ExeTime_intheend;
  double ExeSerialTime, piEstimate;
  

   
   if (argc == 3) {
       thread_count = strtol(argv[1], NULL, 10);  
       if (thread_count <= 0) {
           fprintf(stderr, "   number of threads should be >= 1\n");
       }
       printf("Provided total of threads: %d\n", thread_count);
       
       totalToss = strtoll(argv[2], NULL, 10);
       if (totalToss <= 0) {
           fprintf(stderr, "   totalToss is the number of tosses and should be >= 1\n");
       }
       printf("Provided total of Toss: %lld\n", totalToss);
   } else {
       fprintf(stderr, "usage: %s <number of threads> <totalToss>\n", argv[0]);
       
   }
   
    thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t)); 
    pthread_mutex_init(&mutex, NULL);
    
    A_ExeTime_eachstep = (double*) malloc (thread_count*sizeof(double)); 
    A_ExeTime_intheend = (double*) malloc (thread_count*sizeof(double)); 

   
    for(thread_increment=1; thread_increment <= thread_count; thread_increment++){
    GET_TIME(start);
		g_sumTossInCircle = 0;
		g_numberOfTossesSoFar = 0;
		rankToss = (long long int) totalToss/thread_increment;
		
		for (thread = 0; thread < thread_increment; thread++)  
		   pthread_create(&thread_handles[thread], NULL, do_Tossing_Update_each_step, (void*)rankToss);  
		
		for (thread = 0; thread < thread_increment; thread++) 
		   pthread_join(thread_handles[thread], NULL); 
		piEstimate = 4*g_sumTossInCircle/((double) (totalToss));
    GET_TIME(finish);

    A_ExeTime_eachstep[thread_increment-1] = finish - start;
    }
    printf("\n***************** Result of PI ESTIMATION *****************\n");
    printf("\n* g_sumTossInCircle updated at each step...*\n");
    printf("\n                    %4.8f\n", piEstimate);
    
    printf("\n Exe. Time for case when updating at each step. \n ");
    printf("\n The results are shown in the format: (No. of threads) --->  (Exe. Time) \n\n");
    for(thread_increment=1; thread_increment <= thread_count; thread_increment++){
        printf("\n Thread: %d ---> Exe. Time: %e \n", thread_increment, A_ExeTime_eachstep[thread_increment-1]);
    }

    for(thread_increment=1; thread_increment <= thread_count; thread_increment++) {
    GET_TIME(start);
		g_sumTossInCircle = 0;
		g_numberOfTossesSoFar = 0;
		rankToss = (long long int) totalToss/thread_increment;
		
		for (thread = 0; thread < thread_increment; thread++)  
		   pthread_create(&thread_handles[thread], NULL, do_Tossing_Update_at_End, (void*)rankToss);  
		
		for (thread = 0; thread < thread_increment; thread++) 
		   pthread_join(thread_handles[thread], NULL); 
		piEstimate = 4*g_sumTossInCircle/((double) (totalToss));
		
    GET_TIME(finish);
    
    A_ExeTime_intheend[thread_increment-1] = finish - start;
    }
    
    printf("\n***************** Result of PI ESTIMATION *****************\n");
    printf("\n* g_sumTossInCircle updated at end of calculation...*\n");
    printf("\n                    %4.8f\n", piEstimate);
    
    
    printf("\n\n\n Exe. Time for case when updating in the end. \n ");
    printf("\n The results are shown in the format: (No. of threads) --->  (Exe. Time) \n\n");
    for(thread_increment=1; thread_increment <= thread_count; thread_increment++){
        printf("\n Thread: %d ---> Exe. Time: %e \n", thread_increment, A_ExeTime_intheend[thread_increment-1]);
    }

    GET_TIME(start);
		g_sumTossInCircle = 0;
		g_numberOfTossesSoFar = 0;
		do_Tossing_Serial((void*)totalToss);
		piEstimate = 4*g_sumTossInCircle/((double) (totalToss));
    GET_TIME(finish); 
    
    ExeSerialTime = finish - start;
    printf("\n***************** Result of PI ESTIMATION *****************\n");
    printf("\n* In serial calculation...*\n");
    printf("\n                    %4.8f\n", piEstimate);
    
    FILE *fp;
    fp=fopen("ExecTime.dat", "w");
    fprintf(fp, "Thread     SerialExec     ParallelExecEachStep    ParallelExecEnd\n");
     for(thread_increment=1; thread_increment <= thread_count; thread_increment++){
        fprintf(fp, "%d      %4.4f     %4.4f    %4.4f\n",  thread_increment, ExeSerialTime, A_ExeTime_eachstep[thread_increment-1], A_ExeTime_intheend[thread_increment-1]);
     }
    
    fclose(fp);
    
    
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    return 0;
} /* main */

void* do_Tossing_Update_each_step (void* rankToss) {
    
    long long int numberInCircle, myRankToss;
    int toss;
    double distanceSquared, x, y;
    
    myRankToss = (long long int) rankToss;
    //printf("myRankToss in do_Tossing_Update_each_step: %lld\n",myRankToss);
    numberInCircle = 0;
    for(toss=0; toss<myRankToss; toss++){
      x = rand01();
      y = rand01();
      //printf("Radom value generated: %4.4f  %4.4f\n",x, y);
      distanceSquared = x*x + y*y;
      if (distanceSquared <= 1) {
        numberInCircle++;
      }
      
    pthread_mutex_lock(&mutex);
        if (distanceSquared <= 1) {
            g_sumTossInCircle++;
        }
        g_numberOfTossesSoFar++;
    pthread_mutex_unlock(&mutex);
    }
    return NULL;
} /* do_Tossing_Update_each_step */

void* do_Tossing_Update_at_End (void* rankToss) {
    
    long long int numberInCircle, myRankToss;
    int toss;
    double distanceSquared, x, y;
    
    myRankToss = (long long int) rankToss;
    //printf("myRankToss: %lld\n",myRankToss);
    numberInCircle = 0;
    for(toss=0; toss<myRankToss; toss++){
      x = rand01();
      y = rand01();
      //printf("Radom value generated: %4.4f  %4.4f\n",x, y);
      distanceSquared = x*x + y*y;
      if (distanceSquared <= 1) {
        numberInCircle++;
      }
    } 
    pthread_mutex_lock(&mutex);
        g_sumTossInCircle+=numberInCircle;
        g_numberOfTossesSoFar+=toss;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
} /* do_Tossing_Update_at_End */

void* do_Tossing_Serial (void* rankToss) {
    
    long long int numberInCircle, myRankToss;
    int toss;
    double distanceSquared, x, y;
    
    myRankToss = (long long int) rankToss;
    printf("myRankToss in do_Tossing_Serial: %lld\n",myRankToss);
    numberInCircle = 0;
    for(toss=0; toss<myRankToss; toss++){
      x = rand01();
      y = rand01();
      //printf("Radom value generated: %4.4f  %4.4f\n",x, y);
      distanceSquared = x*x + y*y;
      if (distanceSquared <= 1) {
        numberInCircle++;
      }
    } 
    
    g_sumTossInCircle+=numberInCircle;
    g_numberOfTossesSoFar+=toss;
    
    return NULL;
} /* do_Tossing_Update_at_End */

double rand01()
{
    struct timeval tm;
    
    gettimeofday(&tm, NULL);
    srand(tm.tv_usec * tm.tv_sec);
    return (double)rand() / (double)RAND_MAX  ;
} /* rand01 */

