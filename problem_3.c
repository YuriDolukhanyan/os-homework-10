#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 8

int buffer[BUFFER_SIZE];
int in_pos = 0;
int out_pos = 0;

int P, C;
int K = 0;

int produced_count = 0;
int consumed_count = 0;
int total_items;

sem_t empty_slots;
sem_t full_slots;

pthread_mutex_t buffer_mutex;
pthread_mutex_t count_mutex;

void* produce(void* arg) {
	while (1) {
		pthread_mutex_lock(&count_mutex);
		if (produced_count >= total_items) {
			pthread_mutex_unlock(&count_mutex);
			return NULL;
		}
		++produced_count;
		pthread_mutex_unlock(&count_mutex);
		int item = 1;
		sem_wait(&empty_slots);
		pthread_mutex_lock(&buffer_mutex);
		buffer[in_pos] = item;
		in_pos = (in_pos + 1) % BUFFER_SIZE;
		pthread_mutex_unlock(&buffer_mutex);
		sem_post(&full_slots);
	}
}

void* consume(void* arg) {
	while (1) {
		pthread_mutex_lock(&count_mutex);
		if (consumed_count >= total_items) {
			pthread_mutex_unlock(&count_mutex);
			return NULL;
		}
		++consumed_count;
		pthread_mutex_unlock(&count_mutex);
		sem_wait(&full_slots);
		pthread_mutex_lock(&buffer_mutex);
		int item = buffer[out_pos];
		out_pos = (out_pos + 1) % BUFFER_SIZE;
		pthread_mutex_unlock(&buffer_mutex);
		sem_post(&empty_slots);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("input usage: P C K (optional)...");
		return 1;
	}

	P = atoi(argv[1]);
	C = atoi(argv[2]);
	K = atoi(argv[3]);

	total_items = P * K;

	pthread_t producers[P];
	pthread_t consumers[C];

	pthread_mutex_init(&buffer_mutex, NULL);
	pthread_mutex_init(&count_mutex, NULL);

	sem_init(&empty_slots, 0, BUFFER_SIZE);
	sem_init(&full_slots, 0, 0);

	for (int i = 0; i < P; ++i)
		pthread_create(&producers[i], NULL, produce, NULL);

	for (int i = 0; i < C; ++i)
		pthread_create(&consumers[i], NULL, consume, NULL);

	for (int i = 0; i < P; ++i)
		pthread_join(producers[i], NULL);

	for (int i = 0; i < C; i++)
		pthread_join(consumers[i], NULL);

	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&count_mutex);
	sem_destroy(&empty_slots);
	sem_destroy(&full_slots);

	printf("done: produced = %d, consumed %d\n", produced_count, consumed_count);

	return 0;
}
