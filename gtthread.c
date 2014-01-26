#include <gtthread.h>
#include <ucontext.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREAD_SIZE 20 /* Number of threads that can co-exist */
#define STACKSIZE 1024

typedef struct {
	gtthread_t gtthread_id;
	void *return_value;
	ucontext_t context;
	int finished;

}gtthreads;

gtthreads  threads[MAX_THREAD_SIZE];

static int i = 0; /*For loop counter */
static long quantum;
struct itimerval timer;

static int number_total_threads=0;
static int current_thread=-1;

/* Not sure what else to do with init */
/* Does this timer seem proper? */

void gtthread_init(long period) {
	quantum = period;

	for(i = 0; i<MAX_THREAD_SIZE; i++) {
		threads[i].gtthread_id = -1;
		threads[i].finished = 0;
		id_map[i] = 0;
	}

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = quantum;
	timer.it_interval = timer.it_value;

	sa_sigaction = schedule_handler;
	sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&sa_sigaction.sa_mask);


}
void scheduler() {
	if(number_total_threads > 0) {
		int temp = current_thread;
		while(threads[current_thread].finished == 1) {
			current_thread = (current_thread+1)%number_total_threads;
		}
		swap_context(&threads[temp].context, &threads[current_thread].context);
	}
	return;

}
void schedule_handler() {
	/*turn off timer while you change threads */
	timer.it_value = 0;
	timer.it_interval = timer.it_value;

	/* need to stop the current thread and begin the next thread */
	scheduler();

	
	/*restart the timer */
	timer.it_value.tv_usec = quantum;
	timer.it_interval = timer.it_value;
	sa_sigaction = schedule_handler;
	sa_flags = sa_restart | sa_siginfo;
	sigemptyset(&sa_sigaction.sa_mask);

}

int gtthread_equals(gtthread_t t1, gtthread_t t2) {
	return t1==t2 ? 1:0;
}

/* not sure how to properly exit */
void gtthread_exit(void *ret_val) {
	threads[current].return_value = ret_val;
	gtthread_cancel(current_thread);
	/* setcontext() */
}
int gtthread_cancel(gtthread_t thread_id) {

	int index = 0;
	int found = 0;
	for(i = 0; i<number_total_threads; i++ ) {
		if(gtthreads[i].thread_id == thread_id) {
			index = i;
			found = 1;
			break;
		}
	}
	if(found) {
		threads[index].finished = 1;
		free(threads[index].context.uc_stack.ss_sp);
		number_total_threads--;
		return 1; 
	}
	else {
		printf("Cannot cancel/exit a thread that does not exit\n");
		exit(0);
	}
	return 0;
}
gtthread gtthread_self(void) {
	return  threads[current_thread].gtthread_id;
}

int gtthread_join(gtthread_t thread, void **status) {

	int found = 0;
	int index;
	for(i = 0; i<number_total_threads; i++) {
		if(threads[i].gtthread_id == thread) {
			index = i;
			found = 1;
			break;
		}
	}
	if(found && status != NULL) {
		while(true) {
			if(threads[index].finished == 1) 
				break;
		}
		status = threads[index].return_value;
		return 1;
	}
	else {
		printf("Cannot join a thread that doesn't exit \n");
		exit(0);
	}
	return 0;
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg) {

	int index;
	int possible = 0;
	for(i = 0; i<number_total_threads; i++) {
		if(threads[i] != -1) {
			threads[i].gtthread_id = thread;
			index = i;
			possible = 1;
			break;
		}
	}
	if(possible) {
		threads[index].context.uc_stack.ss_sp = malloc(STACKSIZE);
		threads[index].context.uc_stack.ss_size = STACKSIZE;
		threads[index].context.uc_stack.ss_flags = 0;
		threads[index].finished = 0;
		number_total_threads++;
		return 1;
	}
	else {
		printf("Cannot create any more threads, have reach thread limit \n");
		exit(0);
	}
	return 0;
}


int gtthread_mutex_init(gtthread_mutex_t *mutex) {
	mutex -> lock = 0;
	mutex -> count = 1;
	mutex -> unlock = 0;
}
int gtthread_mutex_lock(gtthread_mutex_t *mutex) {
	
	if(mutex->count > 0 && mutex->lock == 0 && mutex->owner == 0) { 
		mutex->count = 0;
		mutex->lock = 1;
		mutex -> owner = threads[current_thread].gtthread_id;
		while(mutex->count == 0);
	}
	return 0;
}
int gtthread_mutex_unlock(gtthread_mutex_t *mutex) {
	
	if(mutex->owner == threads[current_thread]) {
		mutex->count = 1;
		mutex -> owner = 0;
		mutex -> unlock = 0;
	}
	return 0;
}

	


/* Am I suppose to use get/set context? */
/* How does everything just switch around? */
