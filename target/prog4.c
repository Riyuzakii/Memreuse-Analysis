#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE (1 << 20)

int *a, num_threads;
unsigned long long *private_sum, sum=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *work (void *param)
{
	int i, id = *(int*)(param), j;
	private_sum[id] = 0;
	
        j = (SIZE/num_threads)*(id+1);
	for (i=(SIZE/num_threads)*id; i<j; i++) {
		private_sum[id] += a[i];
        }

        pthread_mutex_lock(&mutex);
        sum += private_sum[id];
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
	private_sum = (unsigned long long*)malloc(num_threads*sizeof(unsigned long long));
        tid = (int*)malloc(num_threads*sizeof(int));
        for (i=0; i<num_threads; i++) tid[i] = i;

        a = (int*)malloc(SIZE*sizeof(int));
	for (i=0; i<SIZE; i++) a[i] = i;

	pthread_attr_init(&attr);

	for (i=1; i<num_threads; i++) {
                /* pthread_create arguments: thread pointer,
                                             attribute pointer,
                                             function pointer,
                                             argument pointer to the function
                */
		pthread_create(&threads[i], &attr, work, &tid[i]);
   	}

	work ((void*)&tid[0]);
	
	for (i=1; i<num_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("SUM: %llu\n", sum);

	return 0;
}
