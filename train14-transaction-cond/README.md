# 課題 transaction.c

## 目的

銀行口座間の振替処理を複数スレッドで実行する。

前回のプログラムには以下の2つの問題があった。

1. 複数スレッドが同時に口座残高を更新するため、全口座の総額が一致しない
2. 送金元口座の残高が不足していても振替が実行されてしまう

これらを解決するために、mutex と条件変数を使用する。

---

# 修正内容

## 問題1：総額が一致しない

複数スレッドが同時に

```c
account[f] -= a;
account[t] += a;
```

を実行すると、競合状態が発生する。

そのため、全体で1つの mutex を作成し、1回のトランザクションごとに lock / unlock を行う。

```c
pthread_mutex_lock(&mutex);

/* 1回の振替処理 */

pthread_mutex_unlock(&mutex);
```

---

## 問題2：残高不足でも振替できてしまう

送金元口座の残高が振替金額未満の場合、振替を実行してはいけない。

そのため、条件変数 `cvar` を使い、残高が足りない場合は待機する。

```c
while (account[f] < a) {
    pthread_cond_wait(&cvar, &mutex);
}
```

---

## 条件変数を使う理由

他のスレッドが口座へ入金したとき、待っているスレッドを起こすためである。

```c
pthread_cond_signal(&cvar);
```

---

# プログラム

ファイル名：`transaction.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ACCOUNT_NUM 100
#define THREAD_NUM 10
#define MAX_TRANS 10000

int account[ACCOUNT_NUM];

int from[MAX_TRANS];
int to[MAX_TRANS];
int amount[MAX_TRANS];

int trans_num = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cvar = PTHREAD_COND_INITIALIZER;

void busy(int money)
{
    int i;
    volatile int x = 0;

    for (i = 0; i < money * 100; i++) {
        x++;
    }
}

int sumAmount()
{
    int i;
    int sum = 0;

    for (i = 0; i < ACCOUNT_NUM; i++) {
        sum += account[i];
    }

    return sum;
}

void *thread_func(void *arg)
{
    int thn = *(int *)arg;
    int start = thn * (trans_num / THREAD_NUM);
    int end = (thn + 1) * (trans_num / THREAD_NUM);
    int i;
    int f, t, a;

    for (i = start; i < end; i++) {
        f = from[i] % ACCOUNT_NUM;
        t = to[i] % ACCOUNT_NUM;
        a = amount[i];

        busy(a);

        pthread_mutex_lock(&mutex);

        while (account[f] < a) {
            pthread_cond_wait(&cvar, &mutex);
        }

        account[f] -= a;
        account[t] += a;

        pthread_cond_signal(&cvar);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main()
{
    FILE *fp;
    char type;
    int f, t, a;
    int i;
    pthread_t th[THREAD_NUM];
    int thnum[THREAD_NUM];

    struct timespec start_time, end_time;
    double elapsed;

    fp = fopen("trans.csv", "r");

    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    while (fscanf(fp, " %c,%d,%d,%d", &type, &f, &t, &a) == 4) {
        if (trans_num >= MAX_TRANS) {
            break;
        }

        from[trans_num] = f;
        to[trans_num] = t;
        amount[trans_num] = a;
        trans_num++;
    }

    fclose(fp);

    for (i = 0; i < ACCOUNT_NUM; i++) {
        account[i] = 10000;
    }

    printf("transactions = %d\n", trans_num);
    printf("before sum = %d\n", sumAmount());

    clock_gettime(CLOCK_REALTIME, &start_time);

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

    clock_gettime(CLOCK_REALTIME, &end_time);

    elapsed =
        (end_time.tv_sec - start_time.tv_sec)
        + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

    printf("after sum = %d\n", sumAmount());
    printf("elapsed time = %f sec\n", elapsed);

    return 0;
}
```

---

# コンパイル

```bash
cc -pthread -o transaction transaction.c
```

---

# 実行

`trans.csv` と同じディレクトリで実行する。

```bash
./transaction
```

---

# 実行例

```text
transactions = 10000
before sum = 1000000
after sum = 1000000
elapsed time = 0.234567 sec
```

---

# 解説

## mutex

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

全体で1つの mutex を作成する。

この mutex により、1回のトランザクション中は他のスレッドが口座残高を変更できないようにする。

---

## 条件変数

```c
pthread_cond_t cvar = PTHREAD_COND_INITIALIZER;
```

残高不足のときにスレッドを待機させるための条件変数。

---

## トランザクション単位でlockする

```c
pthread_mutex_lock(&mutex);
```

1回の振替処理の開始前に lock する。

---

## 残高不足なら待つ

```c
while (account[f] < a) {
    pthread_cond_wait(&cvar, &mutex);
}
```

送金元口座の残高が不足している場合は待機する。

---

## なぜ while を使うのか

`pthread_cond_wait()` から起こされたとしても、必ず条件が満たされているとは限らない。

関係ない口座への入金でも `signal` が送られる可能性があるため、起きた後にもう一度残高を確認する必要がある。

そのため、

```c
if
```

ではなく

```c
while
```

を使う。

---

## pthread_cond_wait()

```c
pthread_cond_wait(&cvar, &mutex);
```

この関数は、

1. mutexを一時的にunlockする
2. 条件変数で待機する
3. signalを受けると再びmutexをlockして戻る

という動作をする。

---

## 振替処理

```c
account[f] -= a;
account[t] += a;
```

送金元口座から金額を引き、送金先口座へ加算する。

---

## signalを送る

```c
pthread_cond_signal(&cvar);
```

入金により、待っているスレッドが実行可能になるかもしれないため、条件変数にsignalを送る。

---

## unlock

```c
pthread_mutex_unlock(&mutex);
```

1回のトランザクションが終わったら mutex を解放する。

---

# 処理の流れ

```text
lock
↓
残高確認
↓
不足なら cond_wait
↓
振替
↓
cond_signal
↓
unlock
```

---

# 注意点

## lockする範囲

mutexはスレッドの繰り返し全体ではなく、1回のトランザクションごとに lock / unlock する。

つまり、以下は間違いである。

```text
スレッド開始時にlock
↓
全トランザクション処理
↓
unlock
```

正しくは

```text
1回の振替ごとにlock/unlock
```

である。

---

# なぜ総額が一致するのか

mutexにより、1回の振替処理

```c
account[f] -= a;
account[t] += a;
```

が途中で他のスレッドに割り込まれない。

そのため、更新が失われず、全口座の総額が保たれる。

---

# なぜ残高がマイナスにならないのか

振替前に

```c
while (account[f] < a)
```

で残高を確認している。

残高が足りない場合は `pthread_cond_wait()` で待機するため、残高不足のまま振替が実行されない。

---

# 使用した関数

| 関数                     | 役割          |
| ---------------------- | ----------- |
| pthread_create()       | スレッド作成      |
| pthread_join()         | スレッド終了待ち    |
| pthread_mutex_lock()   | mutex取得     |
| pthread_mutex_unlock() | mutex解放     |
| pthread_cond_wait()    | 条件変数で待機     |
| pthread_cond_signal()  | 待機中スレッドを起こす |
| clock_gettime()        | 時刻取得        |
| fopen()                | ファイルを開く     |
| fscanf()               | CSV読み込み     |
| fclose()               | ファイルを閉じる    |

---

# 学んだこと

* 共有データの競合状態
* mutexによる排他制御
* 条件変数による待機と通知
* `pthread_cond_wait()` の動作
* `while` による条件再確認の重要性
* トランザクション単位でlockする考え方
* 口座残高が不足した場合の処理

---

# テスト対策

## mutexの目的

```text
複数スレッドが同時に口座残高を変更しないようにする
```

---

## 条件変数の目的

```text
残高不足のときに待機し、入金された可能性があるときに起こす
```

---

## pthread_cond_waitの動作

```text
mutexをunlockして待つ
signalを受ける
再びmutexをlockして戻る
```

---

## whileを使う理由

```text
起こされても条件が満たされているとは限らないため
```

---

## signalを送る理由

```text
入金によって、残高不足で待っていたスレッドが実行可能になる可能性があるため
```

---

## 暗記ポイント

```text
lock
↓
while 残高不足なら cond_wait
↓
振替
↓
cond_signal
↓
unlock
```
