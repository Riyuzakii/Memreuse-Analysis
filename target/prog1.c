#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#define SIZE (1 << 20)
#define ACCESS_PER_THREAD 4

int *a, num_threads;
pthread_mutex_t *mutex;

void *work (void *param)
{
	int i, tid = *(int*)(param), index = (SIZE/num_threads)*tid;
	
	for (i=0; i<(SIZE/num_threads)*ACCESS_PER_THREAD; i++) {
		pthread_mutex_lock(&mutex[index]);
		a[index] += tid;
		pthread_mutex_unlock(&mutex[index]);
		index = (index + tid + 1) % SIZE;
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
 	mutex = (pthread_mutex_t*)malloc(SIZE*sizeof(pthread_mutex_t));
	assert(mutex != NULL);
	for (i=0; i<SIZE; i++) pthread_mutex_init(&mutex[i], NULL);
	
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
