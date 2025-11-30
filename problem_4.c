#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int N = 0;
sem_t semA, semB, semC;
pthread_mutex_t mutex;

void* thread_func(void* arg) {
	char c = *(char*)arg;
	for (int i = 0; i < N; ++i) {
        	if (c == 'A') sem_wait(&semA);
        	else if (c == 'B') sem_wait(&semB);
        	else sem_wait(&semC);

        	pthread_mutex_lock(&mutex);
        	printf("%c %d ", c, i);
        	pthread_mutex_unlock(&mutex);

        	if (c == 'A') sem_post(&semB);
        	else if (c == 'B') sem_post(&semC);
        	else sem_post(&semA);
    	}
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("error, usage: N");
		return 1;
	}

	N += atoi(argv[1]);
	char A = 'A';
	char B = 'B';
	char C = 'C';

	pthread_t thread_A, thread_B, thread_C;

	sem_init(&semA, 0, 1);
	sem_init(&semB, 0, 0);
	sem_init(&semC, 0, 0);

	pthread_create(&thread_A, NULL, thread_func, &A);
	pthread_create(&thread_B, NULL, thread_func, &B);
	pthread_create(&thread_C, NULL, thread_func, &C);

	pthread_join(thread_A, NULL);
	pthread_join(thread_B, NULL);
	pthread_join(thread_C, NULL);

	sem_destroy(&semA);
	sem_destroy(&semB);
	sem_destroy(&semC);

	return 0;
}
