# 小問題 pthread演習

## 目的

POSIXスレッド（pthread）を利用して、親スレッドと子スレッドを同時に実行する方法を学ぶ。

---

# 問題内容

スレッドを使用したプログラム `thread.c` を作成する。

* 親スレッドは `"parent"` と表示し、1秒スリープする処理を10回繰り返す
* 子スレッドは `"child"` と表示し、2秒スリープする処理を5回繰り返す

---

# プログラム

ファイル名：`thread.c`

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *thread_func(void *arg)
{
    int i;

    for (i = 0; i < 5; i++) {
        printf("child\n");
        sleep(2);
    }

    return NULL;
}

int main()
{
    pthread_t th;
    int i;

    if (pthread_create(&th, NULL, thread_func, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    for (i = 0; i < 10; i++) {
        printf("parent\n");
        sleep(1);
    }

    pthread_join(th, NULL);

    return 0;
}
```

---

# コンパイル

pthreadを利用するため、`-pthread` オプションが必要である。

```bash
cc -pthread -o thread thread.c
```

---

# 実行

```bash
./thread
```

---

# 実行例

```text
parent
child
parent
parent
child
parent
parent
child
...
```

表示順は実行ごとに変化する。

---

# 解説

## pthread_create()

```c
pthread_create(&th, NULL, thread_func, NULL);
```

新しいスレッドを生成する関数。

---

### 第1引数

```c
&th
```

生成したスレッドのIDを保存する。

---

### 第3引数

```c
thread_func
```

子スレッドが実行する関数。

---

## 子スレッド

```c
void *thread_func(void *arg)
{
    ...
}
```

スレッド開始時に実行される関数。

---

## 親スレッド

```c
int main()
{
    ...
}
```

main関数が親スレッドとして動作する。

---

## pthread_join()

```c
pthread_join(th, NULL);
```

子スレッドの終了を待機する。

これを書かないと、親スレッドが先に終了してプログラム全体が終了してしまう場合がある。

---

# プロセスとスレッドの違い

## fork()

```text
親プロセス
↓
子プロセス
```

* メモリを共有しない
* PIDが異なる

---

## pthread

```text
親スレッド
↓
子スレッド
```

* 同じプロセス内で動作
* メモリを共有する

---

# sleep()

```c
sleep(1);
```

1秒停止

```c
sleep(2);
```

2秒停止

---

# 使用した関数

| 関数               | 役割       |
| ---------------- | -------- |
| pthread_create() | スレッド作成   |
| pthread_join()   | スレッド終了待機 |
| printf()         | 文字列表示    |
| sleep()          | 待機       |

---

# 学んだこと

* pthreadの基本的な利用方法
* スレッド生成
* スレッド終了待機
* 親スレッドと子スレッドの同時実行
* forkとpthreadの違い

---

# テスト対策

## スレッド生成

```c
pthread_create(&th, NULL, thread_func, NULL);
```

---

## 終了待機

```c
pthread_join(th, NULL);
```

---

## コンパイル

```bash
cc -pthread thread.c
```

---

## 覚えること

```text
pthread_create()
→ スレッド作成

pthread_join()
→ スレッド終了待機

-pthread
→ コンパイル時に必要

fork
→ プロセス生成

pthread
→ スレッド生成
```
