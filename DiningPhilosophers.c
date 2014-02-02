#include <stdio.h>
#include "gtthread.h"

#define N 5
#define EATING 1
#define HUNGRY 1
#define THINKING 1
gtthread_mutex_t g_mutex;
int g_num = 0;
int i;
int main(void) {

	gtthread_t ids[N];
	gtthread_init(1000L);
	gtthread_mutex_init(&g_mutex);

	gtthread_t threads[N];
	for(i = 0; i<N; i++) {
		gtthread_create(&threads[i], actions, NULL);
	}
	while(1);

}
int* test(int num) {
	if(state[num] == HUNGRY && state[(num+4)%N] != EATING && state[(num+1)%N] != EATING) {
		printf("Philosphor %d is eating\n", num+1);
		state[num] = EATING;
		printf("He picked up chopsticks %d and %d \n", (num+1), (num+4)%N);
		return 1;
	}
	return 0;
}
void* actions() {

}
