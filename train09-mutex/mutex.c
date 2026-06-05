#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define THREAD_NUM 4
#define OUTER_COUNT 100
#define INNER_COUNT 10
#define LOOP 10000

struct timespec ts[THREAD_NUM][OUTER_COUNT * INNER_COUNT];
struct timespec start;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void busy()
{
    int i;
    volatile int x = 0;

    for (i = 0; i < LOOP; i++) {
        x++;
    }
}

void *thread_func(void *arg)
{
    int thn = *(int *)arg;
    int i, j;
    int index = 0;

    for (i = 0; i < OUTER_COUNT; i++) {

        pthread_mutex_lock(&mutex);

        for (j = 0; j < INNER_COUNT; j++) {
            busy();
            clock_gettime(CLOCK_REALTIME, &ts[thn][index]);
            index++;
        }

        pthread_mutex_unlock(&mutex);

        busy();
    }

    return NULL;
}

int main()
{
    pthread_t th[THREAD_NUM];
    int thnum[THREAD_NUM];
    int i, j;
    long elapsed;

    clock_gettime(CLOCK_REALTIME, &start);

    for (i = 0; i < THREAD_NUM; i++) {
        thnum[i] = i;

        if (pthread_create(&th[i], NULL, thread_func, &thnum[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(th[i], NULL);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        for (j = 0; j < OUTER_COUNT * INNER_COUNT; j++) {
            elapsed =
                (ts[i][j].tv_sec - start.tv_sec) * 1000000000L
                + (ts[i][j].tv_nsec - start.tv_nsec);

            printf("%ld\t%d\n", elapsed, i);
        }
    }

    return 0;
}