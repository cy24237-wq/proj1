# 有限リソースを取り合うマルチスレッドプログラム

## 目的

mutex を有限個のリソースと考え、複数のスレッドが限られたリソースを取り合う動作を確認する。

今回は、4つのスレッドが3つのリソースから2つを取得できたときだけ処理を実行できるプログラムを作成する。

---

# 課題内容

## プログラム1C

* スレッド数は4つ
* リソース数は3つ
* 各スレッドは3つのリソースのうち2つを取得できたときだけ実行する
* リソースは `pthread_mutex_t` で表す
* `pthread_mutex_trylock()` を使う
* 1つ目を取得できても2つ目を取得できなかった場合は、1つ目を解放して最初からやり直す
* 実行状況を `gnuplot` で確認する

---

# プログラム

ファイル名：`resource_mutex.c`

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define THREAD_NUM 4
#define RESOURCE_NUM 3
#define OUTER_COUNT 100
#define INNER_COUNT 10
#define LOOP 10000

struct timespec ts[THREAD_NUM][OUTER_COUNT * INNER_COUNT];
struct timespec start;

pthread_mutex_t resource[RESOURCE_NUM] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

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
        int first = -1;
        int second = -1;

        while (1) {
            first = -1;
            second = -1;

            for (j = 0; j < RESOURCE_NUM; j++) {
                if (pthread_mutex_trylock(&resource[j]) == 0) {
                    first = j;
                    break;
                }
            }

            if (first == -1) {
                continue;
            }

            for (j = 0; j < RESOURCE_NUM; j++) {
                if (j == first) {
                    continue;
                }

                if (pthread_mutex_trylock(&resource[j]) == 0) {
                    second = j;
                    break;
                }
            }

            if (second != -1) {
                break;
            }

            pthread_mutex_unlock(&resource[first]);
        }

        /*
         * ここがクリティカルセクション。
         * 3つのリソースのうち2つを取得できたスレッドだけが実行できる。
         */
        for (j = 0; j < INNER_COUNT; j++) {
            busy();
            clock_gettime(CLOCK_REALTIME, &ts[thn][index]);
            index++;
        }

        pthread_mutex_unlock(&resource[first]);
        pthread_mutex_unlock(&resource[second]);

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
cc -pthread -o resource_mutex resource_mutex.c
```

---

# 実行

```bash
./resource_mutex
```

---

# 実行結果を保存

```bash
./resource_mutex > result.txt
```

---

# gnuplotで確認

```bash
gnuplot
```

```gnuplot
plot "result.txt" using 1:2 with dots
```

---

# 解説

## mutexをリソースとして考える

通常 mutex は排他制御に使うが、この課題では mutex を1個のリソースとして考える。

```c
pthread_mutex_t resource[3];
```

このように3つの mutex を用意することで、3つのリソースがある状態を表している。

---

# pthread_mutex_trylock()

```c
pthread_mutex_trylock(&resource[j])
```

`pthread_mutex_trylock()` は、mutex を取得できるか試す関数である。

通常の `pthread_mutex_lock()` と違い、取得できない場合でも待ち続けない。

---

## 戻り値

| 戻り値 | 意味          |
| --- | ----------- |
| 0   | mutexの取得に成功 |
| 0以外 | mutexの取得に失敗 |

---

# 2つのリソースを取得する流れ

## 1つ目のリソースを探す

```c
for (j = 0; j < RESOURCE_NUM; j++) {
    if (pthread_mutex_trylock(&resource[j]) == 0) {
        first = j;
        break;
    }
}
```

3つのリソースの中から、取得できるものを1つ探す。

---

## 2つ目のリソースを探す

```c
for (j = 0; j < RESOURCE_NUM; j++) {
    if (j == first) {
        continue;
    }

    if (pthread_mutex_trylock(&resource[j]) == 0) {
        second = j;
        break;
    }
}
```

1つ目とは別のリソースを取得しようとする。

---

## 2つ目が取れなかった場合

```c
pthread_mutex_unlock(&resource[first]);
```

1つ目を解放して、0個の状態からやり直す。

これが重要である。

---

# クリティカルセクション

```c
for (j = 0; j < INNER_COUNT; j++) {
    busy();
    clock_gettime(CLOCK_REALTIME, &ts[thn][index]);
    index++;
}
```

2つのリソースを取得できたスレッドだけが、この処理を実行できる。

ここがクリティカルセクションである。

---

# リソースの解放

```c
pthread_mutex_unlock(&resource[first]);
pthread_mutex_unlock(&resource[second]);
```

処理が終わったら、取得した2つのリソースを解放する。

---

# なぜ1つ目を解放する必要があるのか

1つ目のリソースを持ったまま2つ目を待ち続けると、他のスレッドも同じようにリソースを持ったまま待つ可能性がある。

その結果、全員がリソースを1つずつ持ったまま止まり、処理が進まなくなる。

そのため、2つ目が取れなかった場合は、1つ目を解放して最初からやり直す。

---

# 問題2：プログラム1Bでライブロックになる動作

プログラム1Bでは、2つの mutex を使い、2つとも取得できたスレッドだけが処理を実行する。

`trylock()` を使うため、ロック待ちで停止するデッドロックにはなりにくい。

しかし、書き方によってはライブロックになることがある。

---

## ライブロックとは

ライブロックとは、スレッドは動いているが、処理が進まない状態である。

---

## ライブロックになる例

スレッドAとスレッドBがあるとする。

1. スレッドAが mutex0 を取得する
2. スレッドBが mutex1 を取得する
3. スレッドAは mutex1 を取ろうとするが失敗する
4. スレッドBは mutex0 を取ろうとするが失敗する
5. 両方とも取得済みの mutex を解放する
6. また同時に取り直そうとする
7. 同じ動作を繰り返す

この場合、スレッドは動作しているが、どちらも2つのリソースを取得できず、処理が進まない。

これがライブロックである。

---

# 問題3：プログラム1Cでデッドロックになる動作

プログラム1Cでは、3つのリソースから2つを取得できたスレッドだけが実行できる。

ヒントのように、2つ目が取れなかったときに1つ目を解放しないと、デッドロックになる可能性がある。

---

## デッドロックとは

デッドロックとは、複数のスレッドが互いに必要なリソースを待ち続け、全員が停止してしまう状態である。

---

## デッドロックになる例

リソースが3つある。

```text
resource0
resource1
resource2
```

スレッドA、B、Cが次のように1つずつリソースを取得したとする。

```text
スレッドA → resource0 を取得
スレッドB → resource1 を取得
スレッドC → resource2 を取得
```

各スレッドは2つのリソースが必要なので、もう1つ取得しようとする。

しかし、残りのリソースはすべて他のスレッドが保持している。

```text
スレッドA → resource1 または resource2 を待つ
スレッドB → resource0 または resource2 を待つ
スレッドC → resource0 または resource1 を待つ
```

全員がリソースを1つ持ったまま、他のリソースが空くのを待ち続ける。

結果として、どのスレッドも進めなくなる。

これがデッドロックである。

---

# デッドロックを防ぐ方法

今回のプログラムでは、2つ目のリソースを取得できなかった場合、

```c
pthread_mutex_unlock(&resource[first]);
```

で1つ目を解放する。

これにより、リソースを持ったまま待ち続ける状態を防ぐことができる。

---

# 使用した関数

| 関数                      | 役割              |
| ----------------------- | --------------- |
| pthread_create()        | スレッド作成          |
| pthread_join()          | スレッド終了待ち        |
| pthread_mutex_trylock() | mutexを待たずに取得を試す |
| pthread_mutex_unlock()  | mutexを解放        |
| clock_gettime()         | 現在時刻取得          |
| busy()                  | CPUを使う処理        |

---

# 学んだこと

* mutexをリソースとして考える方法
* `trylock()` の使い方
* 複数リソースを取り合う処理
* クリティカルセクションの考え方
* ライブロックの発生例
* デッドロックの発生例
* 2つ目が取れなかったときに1つ目を解放する理由
* gnuplotによるスレッド実行状況の確認

---

# テスト対策

## trylock

```c
pthread_mutex_trylock(&mutex)
```

待たずにロック取得を試す。

---

## 取得成功

```text
戻り値 0
```

---

## 取得失敗

```text
戻り値 0以外
```

---

## ライブロック

```text
動いているが処理が進まない状態
```

---

## デッドロック

```text
互いに待ち続けて全員停止する状態
```

---

## 重要ポイント

```text
2つ目のリソースが取れなかったら、
1つ目を unlock して最初からやり直す。
```
