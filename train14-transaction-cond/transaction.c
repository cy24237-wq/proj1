#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

#define NUM_ACCOUNTS 100
#define NUM_THREADS 10
#define MAX_TXNS 10000

// 口座情報をまとめる構造体
typedef struct {
    int balance;
    pthread_mutex_t lock;      // 口座ごとの鍵
    pthread_cond_t cvar;      // 口座ごとの条件変数
} account_t;

account_t accounts[NUM_ACCOUNTS];
int from_acc[MAX_TXNS], to_acc[MAX_TXNS], amount[MAX_TXNS];
int N = 0;

// 全口座の総額を計算する関数
long sumAmount() {
    long total = 0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        // 合計計算時も安全のためロックする（簡略化のため一瞬だけ）
        pthread_mutex_lock(&accounts[i].lock);
        total += accounts[i].balance;
        pthread_mutex_unlock(&accounts[i].lock);
    }
    return total;
}

typedef struct {
    int start;
    int end;
} thread_arg_t;

void *transfer(void *arg) {
    thread_arg_t *range = (thread_arg_t *)arg;

    for (int i = range->start; i < range->end; i++) {
        int f = from_acc[i];
        int t = to_acc[i];
        int amt = amount[i];

        // --- Step 1: 送り側の口座をロックして残高確認 ---
        pthread_mutex_lock(&accounts[f].lock);
        
        // 残高が足りない間は待機 (whileで囲むのが鉄則)
        while (accounts[f].balance < amt) {
            pthread_cond_wait(&accounts[f].cvar, &accounts[f].lock);
        }

        // 引き出し処理
        int tmp = accounts[f].balance;
        for (volatile int j = 0; j < amt * 100; j++); // 負荷
        accounts[f].balance = tmp - amt;
        
        pthread_mutex_unlock(&accounts[f].lock);

        // --- Step 2: 受け取り側の口座をロックして入金 ---
        pthread_mutex_lock(&accounts[t].lock);
        
        accounts[t].balance += amt;
        
        // お金が入ったので、この口座で待っているスレッドがいれば起こす
        pthread_cond_signal(&accounts[t].cvar);
        
        pthread_mutex_unlock(&accounts[t].lock);
    }
    return NULL;
}

int main() {
    // 1. 口座とMutex/条件変数の初期化
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].balance = 10000;
        pthread_mutex_init(&accounts[i].lock, NULL);
        pthread_cond_init(&accounts[i].cvar, NULL);
    }

    // 2. CSV読み込み
    FILE *istream = fopen("pj/txn/trans.csv", "r");
    if (!istream) { perror("File error"); return 1; }
    char dummy;
    while (fscanf(istream, "%c,%d,%d,%d\n", &dummy, &from_acc[N], &to_acc[N], &amount[N]) == 4) {
        if (++N >= MAX_TXNS) break;
    }
    fclose(istream);

    long sum_before = sumAmount();
    printf("実行前総額: %ld\n", sum_before);
    tcdrain(1);

    struct timespec start_t, end_t;
    clock_gettime(CLOCK_MONOTONIC, &start_t);

    // 3. スレッド生成
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];
    int chunk = N / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].start = i * chunk;
        args[i].end = (i == NUM_THREADS - 1) ? N : (i + 1) * chunk;
        pthread_create(&threads[i], NULL, transfer, &args[i]);
    }

    // 4. 終了待ち
    for (int i = 0; i < NUM_THREADS; i++) pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end_t);
    long sum_after = sumAmount();

    double sec = (end_t.tv_sec - start_t.tv_sec) + (end_t.tv_nsec - start_t.tv_nsec) / 1e9;
    printf("実行後総額: %ld (%s)\n", sum_after, (sum_before == sum_after) ? "一致しました。" : "不一致です。");
    printf("実行時間: %.6f sec\n", sec);

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
        pthread_cond_destroy(&accounts[i].cvar);
    }

    return 0;
}
