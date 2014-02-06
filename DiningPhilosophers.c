#include <stdio.h>
#include "gtthread.h"

#define N 5
#define EATING 1
#define HUNGRY 2
#define THINKING 3
gtthread_mutex_t g_mutex;
int g_num = 0;
int i;
int state[N];

int main(void) {

	gtthread_t ids[N];
	gtthread_init(1000L);
	gtthread_mutex_init(&g_mutex);

	gtthread_t threads[N];
	for(i = 0; i<N; i++) {
		gtthread_create(&threads[i], main_loop, i);
	}
	while(1);

}
void eat(int num) {
	if(state[num] == HUNGRY && state[(num+4)%N] != EATING && state[(num+1)%N] != EATING) {
		printf("Philosphor %d is eating\n", num+1);
		state[num] = EATING;
		printf("He picked up chopsticks %d and %d \n", (num+1), (num+4)%N);
		sleep(2);
		
	}
	return 0;
}
void* main_loop(int phil) {

	while(1) {
		hungry(phil);
		eat(phil);
		think(phil);

	}		
}
void hungry(int phil) {

	gtthread_mutex_lock(&g_mutex);
	state[phil] = HUNGRY;
	printf("Philosopher %d is hungry \n", phil+1);
	sleep(2);
	eat(phil);
	gtthread_mutex_unlock(&g_mutex);
}
void think(int phil) {

	gtthread_mutex_lock(&g_mutex);
	state[phil] = THINKING;
	printf("Philosopher is putting chopsticks %d and %d down \n", (phil+1), (phil+4)%N);
	printf("Philosopher %d is thinking \n", phil+1);
	sleep(2);
	gtthread_mutex_unlock(&g_mutex);

}
	
	
