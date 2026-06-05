#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define THREAD_NUM 5
#define COUNT 1000
#define LOOP 10000

struct timespec ts[THREAD_NUM][COUNT];
struct timespec start;

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
    int i;

    for (i = 0; i < COUNT; i++) {
        busy();
        clock_gettime(CLOCK_REALTIME, &ts[thn][i]);
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
        for (j = 0; j < COUNT; j++) {
            elapsed =
                (ts[i][j].tv_sec - start.tv_sec) * 1000000000L
                + (ts[i][j].tv_nsec - start.tv_nsec);

            printf("%ld\t%d\n", elapsed, i);
        }
    }

    return 0;
}