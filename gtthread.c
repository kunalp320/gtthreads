long quantum;

#define MAX_SIZE 20
#define STACKSIZE 1024

typedef struct {
	unsigned long int gtthread_id;

}thread_info;

thread_info threads[MAX_SIZE];

void gtthread_init(long period) {
	quantum = period;
	
}
