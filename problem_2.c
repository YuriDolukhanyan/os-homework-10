#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>

long long balance = 0;
bool mode;
bool type;

pthread_mutex_t mtx;
pthread_spinlock_t spl;

void* deposit_solve(void* arg) {
	if (!type) { // short
		if (!mode) { // mutex
			pthread_mutex_lock(&mtx);			
			balance++;
			pthread_mutex_unlock(&mtx);
		} else { // spin
			pthread_spin_lock(&spl);
                        balance++;
                        pthread_spin_unlock(&spl);
		}
	} else { // long
		if (!mode) { // mutex
                        pthread_mutex_lock(&mtx);
                        usleep(100);
			balance++;
                        pthread_mutex_unlock(&mtx);
                } else { // spin
                        pthread_spin_lock(&spl);
                        usleep(100);
			balance++;
                        pthread_spin_unlock(&spl);
                }
	}
	return NULL;
}

void* withdraw_solve(void* arg) {
        if (!type) { // short
                if (!mode) { // mutex
                        pthread_mutex_lock(&mtx);
                        balance--;
                        pthread_mutex_unlock(&mtx);
                } else { // spin
                        pthread_spin_lock(&spl);
                        balance--;
                        pthread_spin_unlock(&spl);
                }
        } else { // long
                if (!mode) { // mutex
                        pthread_mutex_lock(&mtx);
                        usleep(100);
                        balance--;
                        pthread_mutex_unlock(&mtx);
                } else { // spin
                        pthread_spin_lock(&spl);
                        usleep(100);
                        balance--;
                        pthread_spin_unlock(&spl);
                }
        }
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("wrong input, usage ./a.out mutex/spin short/long\n");
		return 1;
	}
	
	if (argv[1][0] == 'm') {
	       mode = false;
	       pthread_mutex_init(&mtx, NULL);
	} else if (argv[1][0] == 's') {
	       mode = true;
	       pthread_spin_init(&spl, PTHREAD_PROCESS_PRIVATE);
	}

	if (argv[2][0] == 's') type = false;
	else if (argv[2][0] == 'l') type = true;

	pthread_t deposit[3];
	pthread_t withdraw[3];

	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	for (int i = 0; i < 3; ++i) {
		pthread_create(&deposit[i], NULL, deposit_solve, NULL);
                pthread_create(&withdraw[i], NULL, withdraw_solve, NULL);
	}

	for (int i = 0; i < 3; ++i) {
		pthread_join(deposit[i], NULL);
                pthread_join(withdraw[i], NULL);
	}

	gettimeofday(&end, NULL);
	double execution_time = (end.tv_sec - start.tv_sec) * 1000.0;
	execution_time += (end.tv_usec - start.tv_usec) / 1000.0;

	if (!mode) pthread_mutex_destroy(&mtx);
	else pthread_spin_destroy(&spl);

	printf("Resulting Balance: %lld\n", balance);
	printf("Apprx runtime: %f\n", execution_time);
	
	return 0;
}
