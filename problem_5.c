#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

int K = 0, N = 0, current_print = 1;
sem_t printers;
pthread_mutex_t count_mutex;

void* print_func(void* arg) {
	unsigned long thread_id = (unsigned long)pthread_self();
	sem_wait(&printers);
	pthread_mutex_lock(&count_mutex);
	printf("thread_id: %lu printing ; currently printing: %d\n", thread_id, current_print++);
	pthread_mutex_unlock(&count_mutex);
	usleep(1000);
	pthread_mutex_lock(&count_mutex);
	--current_print;
	pthread_mutex_unlock(&count_mutex);
	sem_post(&printers);
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("error, correct usage: K N\n");
		return 1;
	}

	K += atoi(argv[1]);
	N += atoi(argv[2]);
	
	pthread_t threads[N];
	sem_init(&printers, 0, K);

	for (int i = 0; i < N; ++i)
		pthread_create(&threads[i], NULL, print_func, NULL);

	for (int i = 0; i < N; i++)
		pthread_join(threads[i], NULL);

	sem_destroy(&printers);
	return 0;
}
