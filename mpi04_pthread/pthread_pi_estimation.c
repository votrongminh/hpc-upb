/****************************************************************************
* pthread_pi_estimation.c
*
* ccsalloc -I -n 1 -t 10m
* gcc -g -Wall -o pthread_pi_estimation pthread_pi_estimation.c -lm -lpthread
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
void* do_Tossing_Update_each_step (void* rankToss);
void* do_Tossing_Update_at_End (void* rankToss);

#define GET_TIME(now) { \
   struct timeval t; \
   gettimeofday(&t, NULL); \
   now = t.tv_sec + t.tv_usec/1000000.0; \
}

int main (int argc, char *argv[]){
  
  double piEstimate, start, finish;
  long long int totalToss, rankToss;
  int thread_count;
  long       thread;  /* Use long in case of a 64-bit system */
  pthread_t* thread_handles;
   
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
    
    GET_TIME(start);
        g_sumTossInCircle = 0;
        g_numberOfTossesSoFar = 0;
        rankToss = (long long int) totalToss/thread_count;
        
        for (thread = 0; thread < thread_count; thread++)  
           pthread_create(&thread_handles[thread], NULL, do_Tossing_Update_each_step, (void*)rankToss);  
        
        for (thread = 0; thread < thread_count; thread++) 
           pthread_join(thread_handles[thread], NULL); 
        
        piEstimate = 4*g_sumTossInCircle/((double) (totalToss));
    GET_TIME(finish);

    printf("\nTotal of g_sumTossInCircle: %lld\n", g_sumTossInCircle);   
    
    printf("\n***************** Result of PI ESTIMATION *****************\n");
    printf("\n* g_sumTossInCircle updated at each step...*\n");
    printf("\n                    %4.8f\n", piEstimate);
    printf("The elapsed time is %e seconds\n", finish - start);
    printf("\n***********************************************************\n\n");
    

    GET_TIME(start);
        g_sumTossInCircle = 0;
        g_numberOfTossesSoFar = 0;
        rankToss = (long long int) totalToss/thread_count;
        
        for (thread = 0; thread < thread_count; thread++)  
           pthread_create(&thread_handles[thread], NULL, do_Tossing_Update_at_End, (void*)rankToss);  
        
        for (thread = 0; thread < thread_count; thread++) 
           pthread_join(thread_handles[thread], NULL); 
        
        piEstimate = 4*g_sumTossInCircle/((double) (totalToss));
    GET_TIME(finish);

    printf("\nTotal of g_sumTossInCircle: %lld\n", g_sumTossInCircle);   
    
    printf("\n***************** Result of PI ESTIMATION *****************\n");
    printf("\n* g_sumTossInCircle updated end of local computation*\n");
    printf("\n                    %4.8f\n", piEstimate);
    printf("The elapsed time is %e seconds\n", finish - start);
    printf("\n***********************************************************\n\n");
    
    GET_TIME(start);
        g_sumTossInCircle = 0;
        g_numberOfTossesSoFar = 0;
        do_Tossing_Update_at_End(totalToss);
    GET_TIME(finish);
    
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    return 0;
} /* main */

void* do_Tossing_Update_each_step (void* rankToss) {
    
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

double rand01()
{
    struct timeval tm;
    
    gettimeofday(&tm, NULL);
    srand(tm.tv_usec * tm.tv_sec);
    return (double)rand() / (double)RAND_MAX  ;
} /* rand01 */

