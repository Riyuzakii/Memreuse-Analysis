#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define SIZE (1 << 20)
#define MAX_FLAGS 16

int *a, num_threads;
volatile char flag[MAX_FLAGS];

void *work (void *param)
{
	int i, j, tid = *(int*)(param);
	int start_index, end_index;

	for (i=0; i<num_threads; i++) {
		start_index = (SIZE/num_threads)*((tid+i)%num_threads);
		end_index = (SIZE/num_threads)*(((tid+i+1) > num_threads) ? (tid+i+1)%num_threads : (tid+i+1));
		for (j=start_index; j<end_index; j++) a[j] += tid;
		flag[(tid+i)%num_threads]++;
		if (i < num_threads - 1) while (flag[(tid+i+1)%num_threads] != (i+1));
	}
}	

int main (int argc, char *argv[])
{
	int i, j, *tid;
	pthread_t *threads;
	pthread_attr_t attr;
	long long unsigned sum = 0;

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

	for (i=0; i<SIZE; i++) sum += a[i];

	printf("SUM: %llu\n", sum);

	return 0;
}
