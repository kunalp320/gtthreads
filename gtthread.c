#include <gtthread.h>
#include <ucontext.h>
#include <sys/time.h>

#define MAX_THREAD_SIZE 20 /* Number of threads that can co-exist */
#define STACK_SIZE 1024 

typedef struct {
	gtthread_t gtthread_id;
	void *return_value;
	ucontext_t context;
	int finished;


}thread_info;

thread_info threads[MAX_THREAD_SIZE];
static int i = 0; /*For loop counter */
static long quantum;
struct itimerval timer;

static int number_total_threads=0;
static int current_thread=-1;

void gtthread_init(long period) {
	quantum = period;

	for(i = 0; i<MAX_THREAD_SIZE; i++) {
		threads[i].gtthread_id = i;
		threads[i].finished = 0;
	}

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = quantum;
	timer.it_interval = timer.it_value;

	sa_sigaction = schedule_handler;
	sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&sa_sigaction.sa_mask);


}
void scheduler() {
	if(number_total_threads >0) {
		int temp = current_thread;
		current_thread = (current_thread+1)%number_total_threads;
		swap_context(&threads[temp].context, &theads[current_thread].context);
	}
	return;

}
void schedule_handler() {
	/*Turn off timer while you change threads */
	timer.it_value = 0;
	timer.it_interval = timer.it_value;

	/* Need to stop the current thread and begin the next thread */
	scheduler();

	
	/*Restart the timer */
	timer.it_value.tv_usec = quantum;
	timer.it_interval = timer.it_value;
	sa_sigaction = schedule_handler;
	sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&sa_sigaction.sa_mask);

}

int gtthread_equals(gtthread_t t1, gtthread_t t2) {
	return t1==t2 ? 1:0;
}
void gtthread_exit(void *ret_val) {
	threads[current].return_value = ret_val;
	gtthread_cancel(current_thread);
	/* setcontext() */
}
int gtthread_cancel(gtthread_t thread_id) {

	number_current_threads--;
	threads[thread_id].finished = 1;
	free(threads[thread_id].context.uc_stack.ss_sp);
	threads[thread_id].finished = 1;
	return 1;

}
gtthread gtthread_self(void) {
	return  threads[current_thread].gtthread_id;
}

int gtthread_join(gtthread_t thread, void **status) {
	while(true) {
		if(threads[(int)thread].finished == 1) {
			break;
		}
	}
	if(status != NULL) {
		status = threads[(int)thread].return_value;
	}
	return;
}



