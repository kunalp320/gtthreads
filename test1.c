#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

/* Tests creation.
      Should print "Hello World!" */

void *thr1(void *in) {

	    printf("Hello World!\n");
	        fflush(stdout);
		    gtthread_exit((int)5);
}

int main() {
	gtthread_t t1;

	gtthread_init(50000L);

	gtthread_create( &t1, thr1, NULL);
	printf("%d\n", (int)t1);
	void ** status = malloc(sizeof(int));
	gtthread_join(t1, status);
	printf("done!\n");
	printf("%d\n", *status);
	return EXIT_SUCCESS;
}
