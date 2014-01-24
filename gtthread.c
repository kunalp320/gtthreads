#include <gtthread.h>
#include <ucontext.h>
#include <sys/time.h>

#define MAX_THREAD_SIZE 20 /* Number of threads that can co-exist */
#define STACK_SIZE 1024 

typedef struct {
	gtthread_t gtthread_id;
	void *return_value;


}thread_info;

thread_info threads[MAX_THREAD_SIZE];
static int i = 0; /*For loop counter */
static long quantum;
struct itimerval timer;

static int next_thread;
static int current_thread;

void gtthread_init(long period) {
	quantum = period;
	
	for(i = 0; i<MAX_THREAD_SIZE; i++) {
		threads[i].gtthread_id = i;
	}

	timer.it_value.tv_sec = 0;
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
	
	free(threads[thread_id]);
	return 1;

}
