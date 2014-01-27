#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

/* Tests creation.
      Should print "Hello World!" */

void *thr1(void *in) {
	    printf("Hello World!\n");
	        fflush(stdout);
		    return NULL;
}

int main() {
	gtthread_t t1;
	printf("HELLO\n");
	gtthread_init(50000L);
	printf("2\n");
	gtthread_create( &t1, thr1, NULL);
	printf("3\n");
	while(1);
	printf("HERE\n");	
	return EXIT_SUCCESS;
}
