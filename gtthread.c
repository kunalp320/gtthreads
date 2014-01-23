#include <gtthread.h>

#define MAX_SIZE 20
#define STACKSIZE 1024

long quantum;

static int i = 0; /* For loop counter */
typedef struct {
	gtthread_t gtthread_id;
	unsigned long int number_times_run;

}thread_info;

thread_info threads[MAX_SIZE];

void gtthread_init(long period) {
	quantum = period;
	
	for(i = 0; i<MAX_SIZE; i++) {
		threads[i].gtthread_id = i;
		threads[i].number_times_run = 0;
	}
	
	

	
	
}
int gtthread_equals(gtthread_t t1, gtthread_t t2) {
	if(t1 == t2) {
		return 1;
	}
	else {
		return 0;
	}
}
