#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define SIZE (1 << 20)
#define MAX_FLAGS 16

int *a, num_threads;
volatile char flag[MAX_FLAGS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long long unsigned sum = 0;

void *work (void *param)
{
	int i, tid = *(int*)(param);
	int start_index = (SIZE/num_threads)*tid;
	int end_index = (SIZE/num_threads)*(tid+1);
	int right_neighbor_start_index = end_index;
	int right_neighbor_end_index = (SIZE/num_threads)*(tid+2);
	int left_neighbor_start_index = (SIZE/num_threads)*(tid-1);
        int left_neighbor_end_index = start_index;
	unsigned long long private_sum = 0;

	for (i=start_index; i<end_index; i++) a[i] += tid;
	flag[tid] = 1;
	if ((tid + 1) < num_threads) {
		while (!flag[tid + 1]);
		for (i=right_neighbor_start_index; i<right_neighbor_end_index; i++) private_sum += a[i];
	}
	if ((tid - 1) >= 0) {
		while (!flag[tid - 1]);
		for (i=left_neighbor_start_index; i<left_neighbor_end_index; i++) private_sum += a[i];
	}

	pthread_mutex_lock(&mutex);
	sum += private_sum;
	pthread_mutex_unlock(&mutex);
}	

int main (int argc, char *argv[])
{
	int i, j, *tid;
	pthread_t *threads;
	pthread_attr_t attr;

	if (argc != 2) {
		printf ("Need number of threads.\n");
		exit(1);
	}
	num_threads = atoi(argv[1]);
	threads = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
	assert(threads != NULL);
        tid = (int*)malloc(num_threads*sizeof(int));
	assert(tid != NULL);
        for (i=0; i<num_threads; i++) tid[i] = i;

        a = (int*)malloc(SIZE*sizeof(int));
	assert(a != NULL);
	for (i=0; i<SIZE; i++) a[i] = i;
	assert(num_threads <= MAX_FLAGS);
	for (i=0; i<MAX_FLAGS; i++) flag[i] = 0;
	
	pthread_attr_init(&attr);

	for (i=1; i<num_threads; i++) {
                /* pthread_create arguments: thread pointer,
                                             attribute pointer,
                                             function pointer,
                                             argument pointer to the function
                */
		pthread_create(&threads[i], &attr, work, &tid[i]);
   	}

	work((void*)&tid[0]);
	
	for (i=1; i<num_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("SUM: %llu\n", sum);

	return 0;
}
