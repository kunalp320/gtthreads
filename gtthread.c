long quantum;

#define MAX_SIZE 20
#define STACKSIZE 1024

static int i = 0; /* For loop counter */
typedef struct {
	unsigned long int gtthread_id;

}thread_info;

thread_info threads[MAX_SIZE];

void gtthread_init(long period) {
	quantum = period;
	
	for(i = 0; i<MAX_SIZE; i++) {
		threads[i].gtthread_id = i;
	}


	
}
