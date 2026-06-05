# 小問題 5threads演習

## 目的

複数のスレッドを同時に実行し、各スレッドが実行された時刻を記録することで、スレッドの実行状況を可視化する。

また、`taskset` を利用して使用するCPUコア数を制限し、スレッドの動作の変化を確認する。

---

# 問題内容

`5threads.c` を作成する。

* main以外に5つのスレッドを作成する
* 各スレッドは処理を繰り返し、現在時刻を配列に記録する
* mainスレッドは全スレッドの終了を待つ
* 最後に「経過時間」と「スレッド番号」を出力する
* 出力結果をgnuplotで表示して、スレッドの動作を確認する

---

# プログラム

ファイル名：`5threads.c`

```c
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
```

---

# コンパイル

```bash
cc -pthread -o 5threads 5threads.c
```

---

# 実行

```bash
./5threads
```

---

# ファイルに保存

```bash
./5threads > result.txt
```

---

# gnuplotで表示

```bash
gnuplot
```

```gnuplot
plot "result.txt" using 1:2 with dots
```

---

# tasksetを使った実験

## CPUコアを2つに制限

```bash
taskset -c 0-1 ./5threads > result2.txt
```

## gnuplotで表示

```gnuplot
plot "result2.txt" using 1:2 with dots
```

---

# 解説

## pthread_create()

```c
pthread_create(&th[i], NULL, thread_func, &thnum[i]);
```

スレッドを作成する。

第4引数でスレッド番号を渡している。

---

## pthread_join()

```c
pthread_join(th[i], NULL);
```

各スレッドの終了を待つ。

---

## clock_gettime()

```c
clock_gettime(CLOCK_REALTIME, &ts[thn][i]);
```

現在時刻を取得する。

取得した時刻は `ts[thn][i]` に保存する。

---

## struct timespec

```c
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
```

| メンバ     | 意味  |
| ------- | --- |
| tv_sec  | 秒   |
| tv_nsec | ナノ秒 |

---

## 経過時間の計算

```c
elapsed =
    (ts[i][j].tv_sec - start.tv_sec) * 1000000000L
    + (ts[i][j].tv_nsec - start.tv_nsec);
```

開始時刻との差をナノ秒単位で計算する。

---

## busy()

```c
void busy()
{
    int i;
    volatile int x = 0;

    for (i = 0; i < LOOP; i++) {
        x++;
    }
}
```

CPUを少し使わせるための処理。

`volatile` を付けることで、コンパイラが処理を省略しにくくしている。

---

# tasksetとは

指定したCPUコアだけでプログラムを実行するコマンド。

```bash
taskset -c 0-1 ./5threads
```

これはCPUコア0と1だけを使って実行するという意味。

---

# 結果の見方

## コア数が多い場合

複数のスレッドが同時に動きやすい。

gnuplotでは、複数のスレッド番号に点が並ぶ。

---

## コア数を減らした場合

実行できるスレッド数が制限されるため、スレッドの実行に隙間ができやすい。

---

# 余力問題1：sched_yield()

## 概要

`sched_yield()` は、現在実行中のスレッドがCPUを一度譲るためのシステムコールである。

---

## 追加する場所

```c
clock_gettime(CLOCK_REALTIME, &ts[thn][i]);
sched_yield();
```

---

## 追加するヘッダ

```c
#include <sched.h>
```

---

## 変化

`sched_yield()` を使うと、現在のスレッドがCPUを譲るため、他のスレッドが実行されやすくなる。

そのため、gnuplotではスレッドの切り替わりが増える可能性がある。

---

# 余力問題2：sched_setscheduler()

## 概要

`sched_setscheduler()` は、プロセスやスレッドのスケジューリング方式を変更するためのシステムコールである。

---

## SCHED_IDLE

```text
CPUが空いているときだけ実行されやすい低優先度の設定
```

あるスレッドだけ `SCHED_IDLE` にすると、そのスレッドの実行回数が少なくなり、gnuplot上で分かりやすく変化する。

---

## SCHED_FIFO

```text
リアルタイムスケジューリング方式の一つ
```

優先度の高いスレッドが実行され続けやすい。

---

# 使用した関数・コマンド

| 名前                   | 役割           |
| -------------------- | ------------ |
| pthread_create()     | スレッド作成       |
| pthread_join()       | スレッド終了待ち     |
| clock_gettime()      | 現在時刻取得       |
| sched_yield()        | CPUを譲る       |
| sched_setscheduler() | スケジューリング方式変更 |
| taskset              | 使用CPUコアを制限   |
| gnuplot              | 結果の可視化       |

---

# 学んだこと

* 複数スレッドの同時実行
* スレッドごとに値を渡す方法
* 時刻を記録する方法
* ナノ秒単位の経過時間計算
* tasksetによるCPUコア制限
* gnuplotによるスレッド実行状況の可視化
* sched_yieldによるCPU譲渡

---

# テスト対策

## スレッド作成

```c
pthread_create(&th[i], NULL, thread_func, &thnum[i]);
```

---

## スレッド番号を渡す

```c
thnum[i] = i;
pthread_create(&th[i], NULL, thread_func, &thnum[i]);
```

---

## 時刻取得

```c
clock_gettime(CLOCK_REALTIME, &ts[thn][i]);
```

---

## 経過時間

```c
elapsed =
    (ts[i][j].tv_sec - start.tv_sec) * 1000000000L
    + (ts[i][j].tv_nsec - start.tv_nsec);
```

---

## gnuplot

```gnuplot
plot "result.txt" using 1:2 with dots
```

---

## taskset

```bash
taskset -c 0-1 ./5threads > result2.txt
```

---

## 暗記ポイント

```text
pthread_create()
→ スレッド生成

pthread_join()
→ 終了待ち

clock_gettime()
→ 現在時刻取得

taskset
→ 使用CPUコア制限

gnuplot
→ 可視化

sched_yield()
→ CPUを譲る
```
