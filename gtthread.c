long quantum;



typedef struct {
	unsigned long int gtthread_id;

}

void gtthread_init(long period) {
	quantum = period;
	
}
