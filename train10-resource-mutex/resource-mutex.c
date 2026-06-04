#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define COUNT 500
struct timespec ts[4][COUNT];
pthread_mutex_t m[3];

long get_nanos(struct timespec x) {
    return (long)x.tv_sec * 1000000000L + x.tv_nsec;
}

void *fun(void *arg) {
    long id = (long)arg;
    int res1 = id % 3;
    int res2 = (id + 1) % 3;
    int index = 0;

    while (index < COUNT) {
        if (pthread_mutex_trylock(&m[res1]) == 0) {
            if (pthread_mutex_trylock(&m[res2]) == 0) {
                // リソースを2つ取れた証拠として時刻を記録
                clock_gettime(CLOCK_REALTIME, &ts[id][index++]);
                
                usleep(1000); // 作業時間（グラフを見やすくするため短めに設定）
                
                pthread_mutex_unlock(&m[res2]);
                pthread_mutex_unlock(&m[res1]);
                usleep(1000); // 休憩
            } else {
                pthread_mutex_unlock(&m[res1]);
                usleep(10);
            }
        }
    }
    return NULL;
}

int main() {
    pthread_t th[4];
    struct timespec start_ts;
    clock_gettime(CLOCK_REALTIME, &start_ts);
    long startt = get_nanos(start_ts);

    for (int i = 0; i < 3; i++) pthread_mutex_init(&m[i], NULL);
    for (long i = 0; i < 4; i++) pthread_create(&th[i], NULL, fun, (void *)i);
    for (int i = 0; i < 4; i++) pthread_join(th[i], NULL);

    for (int thn = 0; thn < 4; thn++) {
        for (int i = 0; i < COUNT; i++) {
            printf("%ld\t%d\n", get_nanos(ts[thn][i]) - startt, thn);
        }
    }
    return 0;
}
