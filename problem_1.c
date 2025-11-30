#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long long counter = 0;

int N = 0;
int M = 0;
char mode;

pthread_mutex_t mtx;
pthread_spinlock_t spl;

void* solve(void* arg) {
	for (int i = 0; i < M; ++i) {
		if (mode == 'A') counter++;
		
		if (mode == 'B') {
			pthread_mutex_lock(&mtx);
			counter++;
			pthread_mutex_unlock(&mtx);
		}

		if (mode == 'C') {
			pthread_spin_lock(&spl);
			counter++;
			pthread_spin_unlock(&spl);
		}
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	// ./a A || B || C N M
	if (argc != 4) {
		printf("A/B/C N M\n");
		return 1;
	}

	mode = argv[1][0];
	N += atoi(argv[2]);
	M += atoi(argv[3]);

	if (mode == 'B')
		pthread_mutex_init(&mtx, NULL);
	else if (mode == 'C')
		pthread_spin_init(&spl, PTHREAD_PROCESS_PRIVATE);

	pthread_t threads[N];
	for (int i = 0; i < N; ++i)
		pthread_create(&threads[i], NULL, solve, NULL);

	for (int i = 0; i < N; ++i)
		pthread_join(threads[i], NULL);

	if (mode == 'B') pthread_mutex_destroy(&mtx);
        if (mode == 'C') pthread_spin_destroy(&spl);

	int expected = N * M;
	printf("Expected: %d\n", expected);
	printf("Actual: %lld\n", counter);

	return 0;
}
