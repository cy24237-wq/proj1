# 小問題 銀行口座トランザクション演習

## 目的

複数スレッドで銀行口座間の振替処理を行い、共有データに対する競合が発生することを確認する。

また、スレッド実行前後で全口座の総額を比較し、並行処理における問題点を理解する。

---

# 問題内容

銀行内に100個の口座があるとする。

各口座の初期金額は

```text
10000円
```

である。

振替データは

```text
~/pj/txn/trans.csv
```

に保存されている。

---

# trans.csv の形式

```text
t,1234,5678,1000
t,2143,6587,2000
...
```

---

## データの意味

```text
t,1234,5678,1000
```

これは

```text
1234番口座から5678番口座へ1000円を振替える
```

という意味である。

`t` は通常のトランザクションを表す。

---

# 行数確認

```bash
wc ~/pj/txn/trans.csv
```

または

```bash
wc -l ~/pj/txn/trans.csv
```

通常は

```text
10000行
```

である。

---

# 作成するプログラム

ファイル名：`bank.c`

## 処理内容

1. `trans.csv` を読み込む
2. `from[N]`, `to[N]`, `amount[N]` に保存する
3. 口座配列 `account[100]` を作成する
4. 各口座の初期値を10000円にする
5. スレッドを10個作成する
6. トランザクションを10分割する
7. 各スレッドが担当範囲の振替を実行する
8. 実行前後で全口座の総額を比較する
9. 実行時間を測定する

---

# プログラム

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

        account[f] -= a;
        account[t] += a;
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

    fp = fopen("/home/ユーザ名/pj/txn/trans.csv", "r");

    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    while (fscanf(fp, " %c,%d,%d,%d", &type, &f, &t, &a) == 4) {
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

# 注意

```c
fp = fopen("/home/ユーザ名/pj/txn/trans.csv", "r");
```

の部分は、自分の環境に合わせて変更する。

例

```c
fp = fopen("/home/haru/pj/txn/trans.csv", "r");
```

または、プログラムを `~/pj/txn/` で実行するなら

```c
fp = fopen("trans.csv", "r");
```

でもよい。

---

# コンパイル

```bash
cc -pthread -o bank bank.c
```

---

# 実行

```bash
./bank
```

---

# 実行例

```text
transactions = 10000
before sum = 1000000
after sum = 998000
elapsed time = 0.123456 sec
```

---

# 本来の正しい結果

口座間の振替なので、銀行全体のお金は増減しない。

そのため、本来は

```text
before sum = after sum
```

になるはずである。

初期値は

```text
100口座 × 10000円 = 1000000円
```

なので、実行前後とも

```text
1000000円
```

になるのが正しい。

---

# なぜ一致しないことがあるのか

複数のスレッドが同時に同じ口座を更新するためである。

例えば、2つのスレッドが同時に同じ口座から引き落としをすると、片方の更新が上書きされて失われる可能性がある。

このような問題を

```text
競合状態
```

または

```text
race condition
```

という。

---

# 振替処理

```c
account[f] -= a;
account[t] += a;
```

この処理は一見単純だが、実際には

```text
読み込み
計算
書き込み
```

に分かれている。

そのため、複数スレッドが同時に実行すると結果が壊れることがある。

---

# 例

## 初期状態

```text
account[0] = 10000
```

---

## スレッドA

```text
account[0] から 1000円引く
```

---

## スレッドB

```text
account[0] から 2000円引く
```

---

## 本来の結果

```text
10000 - 1000 - 2000 = 7000
```

---

## 競合が起きた場合

スレッドAもBも最初に `10000` を読んでしまう。

```text
A: 10000 - 1000 = 9000
B: 10000 - 2000 = 8000
```

最後にBが書き込むと、

```text
account[0] = 8000
```

となり、Aの処理が消える。

---

# busy処理

```c
busy(a);
```

振替前に何もしないループを入れている。

これは処理を遅くして、複数スレッドが同時に実行されやすくするためである。

---

# スレッド分割

```c
int start = thn * (trans_num / THREAD_NUM);
int end = (thn + 1) * (trans_num / THREAD_NUM);
```

トランザクションを10個のスレッドに分割している。

---

# 時間測定

```c
clock_gettime(CLOCK_REALTIME, &start_time);
```

開始時刻を取得。

```c
clock_gettime(CLOCK_REALTIME, &end_time);
```

終了時刻を取得。

---

## 経過時間

```c
elapsed =
    (end_time.tv_sec - start_time.tv_sec)
    + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
```

---

# 使用した関数

| 関数               | 役割       |
| ---------------- | -------- |
| fopen()          | ファイルを開く  |
| fscanf()         | CSVを読み込む |
| fclose()         | ファイルを閉じる |
| pthread_create() | スレッド作成   |
| pthread_join()   | スレッド終了待ち |
| clock_gettime()  | 時刻取得     |
| perror()         | エラー表示    |

---

# 学んだこと

* CSVファイルの読み込み
* 複数スレッドによる分割処理
* 共有配列の更新
* 競合状態
* race condition
* pthread_joinの重要性
* clock_gettimeによる実行時間測定

---

# テスト対策

## この課題の重要点

```text
振替なので本来は総額が変わらない
```

---

## しかし一致しない理由

```text
複数スレッドが同じ口座を同時に更新するため
```

---

## この問題名

```text
競合状態
race condition
```

---

## 解決方法

```text
mutexで口座更新部分を排他制御する
```

---

## 重要コード

```c
account[f] -= a;
account[t] += a;
```

この部分が共有データを更新している。

---

## 暗記ポイント

```text
トランザクション
→ 1回の振替処理

sumAmount
→ 全口座の総額計算

pthread_create
→ スレッド作成

pthread_join
→ スレッド終了待ち

race condition
→ 複数スレッドが同じデータを同時更新して結果が壊れること
```
