#include "gtthread.h"
#include <sys/ucontext.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_THREAD_SIZE 20 /*Number of threads that can co-exist */
#define STACKSIZE 1024

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

/* Not sure what else to do with init */
/* Does this timer seem proper? */
void scheduler();
void schedule_handler();
static void function_catcher();
static void main_context();

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

	return;

}

void scheduler() {

	if(number_total_threads > 0) {
		int temp = current_thread;
	/*	while(threads[current_thread].finished == 1) { */
			current_thread = (++current_thread)%number_total_threads;
		

		if(number_total_threads > 1)  
			swapcontext(&threads[temp].context, &threads[current_thread].context);
		lse {
			setcontext(&threads[0].context);
		}
	}
	return;

}
void schedule_handler() {
	/*turn off timer while you change threads */
	timer.it_value.tv_sec = 0;
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

/* not sure how to properly exit */
void gtthread_exit(void *ret_val) {
	threads[current_thread].return_value = ret_val;
	gtthread_cancel(current_thread);
	makecontext(&threads[(current_thread+1)%number_total_threads]);
}
int gtthread_cancel(gtthread_t thread_id) {

	if((int)thread_id < number_total_threads) {
		threads[(int)thread_id].finished = 1;
		free(threads[(int)thread_id].context.uc_stack.ss_sp); 
		number_total_threads--;
		return 1;
	}
	else {
		printf("Cannot cancel/exit a thread that does not exist \n");
		exit(0);
	}
	return 0;

}
gtthread_t gtthread_self(void) {
	return threads[current_thread].gtthread_id;
}

int gtthread_join(gtthread_t thread, void **status) {

	if((int)thread < number_total_threads && status != NULL) {
		while(1) {
			if(threads[(int)thread].finished == 1)
				break;
		}
		status = threads[(int)thread].return_value;
		return 1;
	}
	else {
		printf("Cannot join a thread that doesn't exist");
		exit(0);
	}
	return 0;
}
static void function_catcher(void *(*start_routine)(void *), void *arg) {


	threads[current_thread].return_value = start_routine(arg);
	int temp = (1+current_thread)%number_total_threads;

	gtthread_cancel(threads[current_thread].gtthread_id);
	swapcontext(&threads[current_thread].context, &threads[temp].context);
	current_thread = temp;
	return;
}
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg) {


	number_total_threads++;
	if(number_total_threads < MAX_THREAD_SIZE) {
		int new_thread = number_total_threads-1;
	

		getcontext(&threads[new_thread].context);

		threads[new_thread].context.uc_stack.ss_sp = malloc(STACKSIZE);
		threads[new_thread].context.uc_stack.ss_size = (STACKSIZE);
		threads[new_thread].context.uc_stack.ss_flags = 0;
		threads[new_thread].context.uc_link = &threads[0].context;
		threads[new_thread].finished = 0;
		makecontext(&threads[new_thread].context, function_catcher, 2, start_routine, arg);
	
		return 1;

		}
	else {
		printf("cannot create any more threads, have reach thread limit \n");
		exit(1);
	}
	return 0;
}


int gtthread_mutex_init(gtthread_mutex_t *mutex) {
	mutex -> lock = 0;
	mutex -> count = 1;
	mutex -> owner = 0;
	return 1;
}
int gtthread_mutex_lock(gtthread_mutex_t *mutex) {
	
	if(mutex->count ==  1 && mutex->lock == 0 && mutex->owner == 0) { 
		mutex -> count = 0;
		mutex -> lock = 1;
		mutex -> owner = threads[current_thread].gtthread_id;
		while(mutex->count == 0);
	}
	else {
		printf("Cannot use the mutex, currently being used \n");
	}
	return 0;
}
int gtthread_mutex_unlock(gtthread_mutex_t *mutex) {
	
	if(mutex -> owner == threads[current_thread].gtthread_id) {
		mutex -> count = 1;
		mutex -> owner = 0;
		mutex -> lock  = 0;
	}
	return 0;
}


