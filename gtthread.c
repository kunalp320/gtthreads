#include "gtthread.h"
#include <ucontext.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_THREAD_SIZE 150 /*Number of threads that can co-exist */
#define STACKSIZE 1024*1024

typedef struct {
        gtthread_t gtthread_id;
        void *return_value;
        ucontext_t context;
        int finished;

}gtthreads;


gtthreads threads[MAX_THREAD_SIZE];


ucontext_t context_link;
static int i = 0; /*For loop counter */
static long quantum;
struct itimerval timer;
struct sigaction sig;
static int number_total_threads=0;
static int current_thread=-1;


void scheduler();
void schedule_handler();
static void function_catcher();


void gtthread_init(long period) {
        quantum = period;

        for(i = 0; i<MAX_THREAD_SIZE; i++) {
                threads[i].gtthread_id = i;
                threads[i].finished = 0;
        }


        /*Create the main thread here */
        number_total_threads++;
        current_thread = 0;

        getcontext(&threads[current_thread].context);
       
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = quantum;
        sig.sa_sigaction = schedule_handler;
	
        sig.sa_flags = SA_RESTART | SA_SIGINFO;
        sigemptyset(&sig.sa_mask); 
        sigaction(SIGPROF, &sig, NULL);
        setitimer(ITIMER_PROF, &timer, NULL);

}

void scheduler() {

        if(number_total_threads > 0) {
	        int temp = current_thread;
		current_thread = (current_thread+1)%number_total_threads;

		while(threads[current_thread].finished == 1) {
			current_thread = (current_thread+1)%number_total_threads;
		}

	       	timer.it_value.tv_sec = 0;
        	timer.it_value.tv_usec = quantum;
   
  	      	setitimer(ITIMER_PROF, &timer, NULL);
	  
	       
		  if(number_total_threads > 1) {	
                        swapcontext(&threads[temp].context, &threads[current_thread].context);
		  }
                
        }
	if(threads[0].finished == 1) {
	  exit(0);
	}
	
	return;
}
void schedule_handler() {
        /*turn off timer while you change threads */
        timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 0;
        timer.it_interval = timer.it_value;

        /* need to stop the current thread and begin the next thread */
        scheduler();

        
        /*restart the timer */
        timer.it_value.tv_usec = quantum;
        timer.it_interval = timer.it_value;
        setitimer(ITIMER_PROF, &timer, NULL);
        return;
}

int gtthread_equals(gtthread_t t1, gtthread_t t2) {
        return t1==t2 ? 1:0;
}
int gtthread_yield(void) {
       
       schedule_handler();
	return 0;
}
/* not sure how to properly exit */
void gtthread_exit(void *ret_val) {

        threads[current_thread].return_value = ret_val;
	threads[current_thread].finished = 1;
	gtthread_yield();
        
	if(current_thread == 0) {
	  exit(1);
	}
        
}
int gtthread_cancel(gtthread_t thread_id) {
	
	int index =-1;
 	for(i = 0; i<number_total_threads; i++ ) {
    		if(threads[i].gtthread_id == (int)thread_id) {
     			 index = i;
      			 break;
   		 }
  	}
	if(index != -1) {

		threads[index].finished = 1;
	
		free(threads[index].context.uc_stack.ss_sp);
	       
		for(i = index; i<number_total_threads-1; i++) {
			threads[i] = threads[i+1];
		}
		number_total_threads--;
		if(number_total_threads == 0) {
		  exit(0);
		}
	
		return 0;
	}
	else {
		printf("Cannot cancel a thread that does not exist\n");
		exit(1);
	}

}

int gtthread_join(gtthread_t thread, void **status) {
	int index = -1;
	for(i = 0; i<number_total_threads; i++) {
		if(threads[i].gtthread_id == (int)thread) {
      			index = i;
      			break;
    		}
  	}
  
  	if(index != -1) {        
	
                while(threads[index].finished != 1) {
			gtthread_yield();
		}
		if(status != NULL) {	
		   *status = threads[index].return_value;
		}
		
                return 0;
        }
        else {
                printf("Cannot join a thread that doesn't exist\n");
                exit(0);
        }
        return 0;
}
static void function_catcher(void *(*start_routine)(void *), void *arg) {
	
	void * ans = start_routine(arg);
	if(threads[current_thread].finished != 1) {
		threads[current_thread].return_value = ans;
		threads[current_thread].finished = 1;
	}
   	schedule_handler();
	
}
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg) {


        number_total_threads++;
        if(number_total_threads < MAX_THREAD_SIZE) {
                int new_thread = number_total_threads-1;

                getcontext(&threads[new_thread].context); 
        
                *thread = threads[new_thread].gtthread_id;
                threads[new_thread].context.uc_stack.ss_sp = malloc(STACKSIZE);
                threads[new_thread].context.uc_stack.ss_size = (STACKSIZE);
                threads[new_thread].context.uc_stack.ss_flags = 0;
                threads[new_thread].finished = 0;
                makecontext(&threads[new_thread].context, function_catcher, 2, start_routine, arg);
        
                return 0;

                }
        else {
                printf("cannot create any more threads, have reach thread limit \n");
                exit(1);
        }
        return 0;
}

gtthread_t gtthread_self(void) {
	
	gtthread_t toReturn =  threads[current_thread].gtthread_id;
	return toReturn;
}
int gtthread_mutex_init(gtthread_mutex_t *mutex) {
        mutex -> lock = 0;
        mutex -> count = 1;
        mutex -> owner = -1;
        return 0;
}
int gtthread_mutex_lock(gtthread_mutex_t *mutex) {
        
        if(mutex->count == 1 && mutex->lock == 0 && mutex->owner == -1) {
                mutex -> count = 0;
                mutex -> lock = 1;
                mutex -> owner = threads[current_thread].gtthread_id;
             
        }
	else {
		while(mutex -> count == 0 && mutex -> lock == 1);
		gtthread_mutex_lock(mutex);
	}
    
        return 0;
}
int gtthread_mutex_unlock(gtthread_mutex_t *mutex) {
        
        if(mutex -> owner == threads[current_thread].gtthread_id && mutex -> lock == 1) {
                mutex -> count = 1;
                mutex -> owner = -1;
                mutex -> lock = 0;
        }
        return 0;
}


