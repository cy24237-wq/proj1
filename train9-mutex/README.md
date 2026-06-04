# 小問題 mutex演習

## 目的

`mutex` を利用して、複数スレッドのうち一度に1つのスレッドだけがクリティカルセクションを実行することを確認する。

また、実行時刻を記録し、`gnuplot` で可視化することで、mutexによる排他制御の動作を理解する。

---

# 問題内容

`5threads.c` をもとにして、mutexを使ったプログラムを作成する。

* スレッド数は4つにする
* 各スレッドは1つのmutexを取り合う
* mutexを取得したスレッドだけが処理Aを実行する
* 処理Aがクリティカルセクションである
* 処理Aでは、`busy()` と時刻測定を10回繰り返す
* mutexを解放した後、処理Bとして `busy()` を1回実行する
* これを100回程度繰り返す
* 結果を `gnuplot` で表示する

---

# プログラム

ファイル名：`mutex.c`

```c
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
```

---

# コンパイル

```bash
cc -pthread -o mutex mutex.c
```

---

# 実行

```bash
./mutex
```

---

# 結果をファイルに保存

```bash
./mutex > result.txt
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

# 解説

## mutexとは

`mutex` は相互排他を行うための仕組みである。

複数のスレッドが同時に同じ処理や共有資源へアクセスしないように制御する。

---

## mutexの宣言

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

mutex変数を作成し、初期化している。

---

## mutexを取得する

```c
pthread_mutex_lock(&mutex);
```

この行でmutexを取得する。

すでに他のスレッドがmutexを持っている場合、このスレッドは待たされる。

---

## クリティカルセクション

```c
for (j = 0; j < INNER_COUNT; j++) {
    busy();
    clock_gettime(CLOCK_REALTIME, &ts[thn][index]);
    index++;
}
```

mutexを取得している間に実行される処理。

この部分がクリティカルセクションである。

一度に1つのスレッドだけが実行できる。

---

## mutexを解放する

```c
pthread_mutex_unlock(&mutex);
```

mutexを解放する。

これにより、他のスレッドがmutexを取得できるようになる。

---

## 処理B

```c
busy();
```

mutexを解放した後に実行する処理。

処理Bを入れる理由は、unlock後に同じスレッドがすぐmutexを取り直してしまうと、gnuplotの表示が分かりにくくなるためである。

---

# 実行結果の見方

gnuplotで表示すると、同じ時間帯に複数のスレッドがクリティカルセクションを実行していないことが確認できる。

つまり、mutexによって

```text
一度に1つのスレッドだけ
```

が処理Aを実行していることが分かる。

---

# 5threads.cとの違い

## 5threads.c

* 複数スレッドが同時に動く
* 各スレッドが自由に時刻を記録する
* 点が重なりやすい

---

## mutex.c

* mutexを取ったスレッドだけが処理Aを実行する
* 同時にクリティカルセクションへ入れない
* gnuplotで見ると、スレッドごとの実行が交互に現れやすい

---

# 使用した関数

| 関数                     | 役割       |
| ---------------------- | -------- |
| pthread_create()       | スレッド作成   |
| pthread_join()         | スレッド終了待ち |
| pthread_mutex_lock()   | mutex取得  |
| pthread_mutex_unlock() | mutex解放  |
| clock_gettime()        | 現在時刻取得   |
| busy()                 | CPUを使う処理 |
| gnuplot                | 可視化      |

---

# 学んだこと

* mutexによる排他制御
* クリティカルセクションの意味
* lockとunlockの使い方
* 複数スレッドの競合を防ぐ方法
* gnuplotによるスレッド実行状況の可視化

---

# テスト対策

## mutexの基本

```c
pthread_mutex_lock(&mutex);

/* クリティカルセクション */

pthread_mutex_unlock(&mutex);
```

---

## mutexの目的

```text
複数スレッドが同時に共有資源へアクセスするのを防ぐ
```

---

## クリティカルセクション

```text
同時に実行されると問題が起きる処理部分
```

---

## 処理Bを入れる理由

```text
unlock後に同じスレッドがすぐmutexを取り直すと、
gnuplotの表示が分かりにくくなるため
```

---

## 暗記ポイント

```text
mutex
→ 相互排他

pthread_mutex_lock()
→ mutexを取得

pthread_mutex_unlock()
→ mutexを解放

クリティカルセクション
→ 一度に1つのスレッドだけが実行すべき部分
```
