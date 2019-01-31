#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STEP 1000 * 1000

size_t number_of_sieves = STEP;
bool *prime;
size_t max_divider;

struct borders {
    size_t left;
    size_t right;
};

void *f(struct borders *arg) {
    for (size_t i = 2; i <= max_divider; ++i) {
        if (!prime[i]) {
            continue;
		}
        size_t new_left = ((arg->left + i - 1) / i) * i;
        for (size_t j = new_left; j <= arg->right; j += i) {
            prime[j] = false;
		}
    }
}


     
int main(int argc, const char * argv[]) {

    time_t start_time = time(NULL);

    if (argc == 1 || argc > 3) {
        perror("wrong input");
        exit(EXIT_FAILURE);
    }
    int max_thread_number = atoll(argv[1]);
    if (argc > 2) {
        number_of_sieves = atoll(argv[2]);
	}

    prime = (bool*) malloc(number_of_sieves);
	prime[0] = false;
    prime[1] = false;
    for (size_t j = 2; j < number_of_sieves; ++j) {
        prime[j] = true;
	}
	bool no_limit = false;
    if (argc == 2) {
    	no_limit = true;
	}

    for (size_t i = 2; (i < number_of_sieves - 1) || no_limit; i*=i) {
        size_t finish = i*i;

		if (no_limit) {
		    if (finish > number_of_sieves - 1) {
		        bool *tmp = realloc(prime, finish + 1);
		        prime = tmp;
		        memset(prime + number_of_sieves, 1, finish + 1 - number_of_sieves);
		        number_of_sieves = finish + 1;
		    }
		} else {
			if (finish > number_of_sieves - 1)
            	finish = number_of_sieves - 1;
		}

        size_t threads_number = finish - i;
        if (threads_number > max_thread_number) {
            threads_number = max_thread_number;
		}
		pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * threads_number);
    	struct borders *bord = (struct borders *) malloc (sizeof(struct borders)*threads_number);

        size_t full_length = finish - i;
        size_t s_threads_number = full_length % threads_number;
        size_t finish_borders = i + 1;
        max_divider = i;
        for (size_t j = 0; j < threads_number; ++j) {
            bord[j].left = finish_borders;
            size_t length = full_length / threads_number;
            if (j < s_threads_number) {
                ++length;
			}
            finish_borders += length - 1;
            bord[j].right = finish_borders;
            ++finish_borders;
            while (pthread_create(threads + j, NULL, f, bord + j));
        }
        for (size_t j = 0; j < threads_number; ++j) {
            pthread_join(threads[j], NULL);
		}

        for (size_t j = i + 1; j!= finish; ++j) {
            if (prime[j]) {
                printf("%ld\n", j);
			}
		}

		time_t curTime = time(NULL) - start_time;
    	double Time = (double)curTime / CLOCKS_PER_SEC;
    	printf("Time : %ld\n", curTime);	

		free(bord);
    	free(threads);
    }
    free(prime);
    return 0;
}
