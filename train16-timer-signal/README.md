# 小問題 timer演習

## 目的

Linuxのタイマー機能を利用して、一定時間ごとにシグナルを発生させる方法を学ぶ。

今回は、main関数では1秒ごとに `"z"` を表示し、同時にタイマーによって1秒ごとに `"hello"` を表示するプログラムを作成する。

---

# 問題内容

`timer.c` を作成する。

第1回で作成した `rec.c` をベースにして、以下の変更を行う。

* シグナル番号を `SIGUSR1` ではなく、タイマー用のシグナル番号に変更する
* 1秒間隔で継続的に起動されるタイマーを設定する
* mainでは1秒ごとに `"z"` を表示する
* タイマーが発生すると `"hello"` を表示する

---

# 使用するシグナル

タイマーで使用するシグナルは

```c
SIGALRM
```

である。

---

# プログラム1：1秒ごとに繰り返すタイマー

ファイル名：`timer.c`

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void timer_handler(int signum)
{
    printf("hello\n");
}

int main()
{
    struct itimerval timer;

    if (signal(SIGALRM, timer_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("setitimer");
        return 1;
    }

    while (1) {
        printf("z\n");
        sleep(1);
    }

    return 0;
}
```

---

# コンパイル

```bash
cc -o timer timer.c
```

---

# 実行

```bash
./timer
```

---

# 実行例

```text
z
hello
z
hello
z
hello
```

---

# 解説

## SIGALRM

```c
SIGALRM
```

タイマーが満了したときに発生するシグナルである。

---

## signal()

```c
signal(SIGALRM, timer_handler);
```

`SIGALRM` を受信したときに `timer_handler()` を実行するように設定する。

---

## ハンドラ

```c
void timer_handler(int signum)
{
    printf("hello\n");
}
```

タイマーのシグナルを受け取ったときに呼び出される関数である。

---

# struct itimerval

```c
struct itimerval timer;
```

タイマーの開始時間と繰り返し間隔を設定するための構造体である。

---

## it_value

```c
timer.it_value.tv_sec = 1;
timer.it_value.tv_usec = 0;
```

最初にタイマーが発火するまでの時間。

この例では、1秒後に最初の `SIGALRM` が発生する。

---

## it_interval

```c
timer.it_interval.tv_sec = 1;
timer.it_interval.tv_usec = 0;
```

2回目以降の繰り返し間隔。

この例では、1秒ごとに継続して `SIGALRM` が発生する。

---

## setitimer()

```c
setitimer(ITIMER_REAL, &timer, NULL);
```

タイマーを開始する関数である。

---

# 処理の流れ

```text
signalでSIGALRMの処理を登録
↓
setitimerで1秒タイマー開始
↓
mainはzを表示し続ける
↓
1秒ごとにSIGALRM発生
↓
timer_handlerが実行されhelloを表示
```

---

# プログラム2：1秒後に1回だけ起動するタイマー

提出課題で使う場合は、1秒後に1回だけ起動するタイマーに変更する。

変更する部分は `it_interval` である。

---

## 変更後のコード

```c
timer.it_value.tv_sec = 1;
timer.it_value.tv_usec = 0;

timer.it_interval.tv_sec = 0;
timer.it_interval.tv_usec = 0;
```

---

## 完成版

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

void timer_handler(int signum)
{
    printf("hello\n");
}

int main()
{
    struct itimerval timer;

    if (signal(SIGALRM, timer_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("setitimer");
        return 1;
    }

    while (1) {
        printf("z\n");
        sleep(1);
    }

    return 0;
}
```

---

# 1回だけ起動する理由

```c
timer.it_interval.tv_sec = 0;
timer.it_interval.tv_usec = 0;
```

にすると、繰り返し間隔が0になる。

そのため、タイマーは最初の1回だけ発生する。

---

# 使用した関数

| 関数          | 役割          |
| ----------- | ----------- |
| signal()    | シグナルハンドラを登録 |
| setitimer() | タイマーを設定     |
| sleep()     | 指定秒数だけ停止    |
| printf()    | 文字列表示       |
| perror()    | エラー表示       |

---

# 使用した構造体

| 構造体              | 役割        |
| ---------------- | --------- |
| struct itimerval | タイマーの時間設定 |

---

# 学んだこと

* タイマーによるシグナル発生
* `SIGALRM` の使い方
* `setitimer()` の使い方
* `it_value` と `it_interval` の違い
* 繰り返しタイマーと1回だけのタイマーの違い
* シグナルハンドラの登録方法

---

# テスト対策

## タイマー用シグナル

```c
SIGALRM
```

---

## シグナル登録

```c
signal(SIGALRM, timer_handler);
```

---

## タイマー設定

```c
setitimer(ITIMER_REAL, &timer, NULL);
```

---

## 最初の発火時間

```c
timer.it_value
```

---

## 繰り返し間隔

```c
timer.it_interval
```

---

## 1秒ごとに繰り返す場合

```c
timer.it_value.tv_sec = 1;
timer.it_interval.tv_sec = 1;
```

---

## 1回だけ起動する場合

```c
timer.it_value.tv_sec = 1;
timer.it_interval.tv_sec = 0;
```

---

## 暗記ポイント

```text
SIGALRM
→ タイマー用シグナル

it_value
→ 最初に起動するまでの時間

it_interval
→ 繰り返し間隔

it_interval = 0
→ 1回だけ起動

setitimer
→ タイマー開始
```
