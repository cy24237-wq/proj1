# 課題：sig.c と rec.c

## 目的

Linuxのシグナル機能を利用して、プロセス間通信を行う方法を学ぶ。

今回の課題では、

* rec.c ：シグナルを受信するプログラム
* sig.c ：シグナルを送信するプログラム

を作成する。

---

# シグナルとは

シグナル（Signal）はソフトウェアによる割り込み機能である。

あるプロセスに対して通知を送ることができる。

今回使用するシグナルは

```text
SIGUSR1
```

である。

---

# rec.c

## 役割

* SIGUSR1を受信する
* 受信したらメッセージを表示する
* 1秒ごとに z を表示する

---

## プログラム

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signum)
{
    printf("signal received\n");
}

int main()
{
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
        perror("signal");
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

## 解説

### シグナルハンドラ

```c
void sig_handler(int signum)
{
    printf("signal received\n");
}
```

シグナル受信時に実行される関数。

---

### signal()

```c
signal(SIGUSR1, sig_handler);
```

意味

```text
SIGUSR1を受信したら
sig_handlerを実行する
```

---

### 無限ループ

```c
while(1)
```

永久に繰り返す。

---

### sleep(1)

```c
sleep(1);
```

1秒待機する。

---

## 実行結果

```text
z
z
z
signal received
z
z
```

---

# sig.c

## 役割

指定したプロセスへSIGUSR1を送信する。

---

## プログラム

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s pid\n", argv[0]);
        return 1;
    }

    pid = atoi(argv[1]);

    if (kill(pid, SIGUSR1) < 0) {
        perror("kill");
        return 1;
    }

    printf("signal sent\n");

    return 0;
}
```

---

## 解説

### argc

```c
argc
```

コマンドライン引数の個数

---

### argv

```c
argv
```

コマンドライン引数の内容

---

### atoi()

```c
atoi(argv[1]);
```

文字列を整数へ変換する。

例

```text
"1234"
↓
1234
```

---

### kill()

```c
kill(pid, SIGUSR1);
```

指定したPIDへSIGUSR1を送信する。

---

## 実行例

```bash
./sig 1234
```

意味

```text
PID 1234 のプロセスへ
SIGUSR1を送信
```

---

# コンパイル

## rec.c

```bash
cc -o rec rec.c
```

---

## sig.c

```bash
cc -o sig sig.c
```

---

# 実行手順

## 1. recを起動

```bash
./rec
```

---

## 2. 別端末を開く

プロセス番号確認

```bash
ps u
```

例

```text
PID
1234
```

---

## 3. sigを実行

```bash
./sig 1234
```

---

## 結果

sig側

```text
signal sent
```

---

rec側

```text
z
z
signal received
z
```

---

# 使用した関数

| 関数       | 役割       |
| -------- | -------- |
| signal() | シグナル受信設定 |
| kill()   | シグナル送信   |
| sleep()  | 待機       |
| atoi()   | 文字列→整数変換 |
| printf() | 表示       |
| perror() | エラー表示    |

---

# プロセス番号(PID)

PIDは実行中プロセスを識別する番号である。

確認

```bash
ps u
```

取得

```c
getpid()
```

---

# 学んだこと

* Linuxのシグナル機能
* SIGUSR1によるプロセス間通信
* signal()によるハンドラ登録
* kill()によるシグナル送信
* PIDの利用方法
* コマンドライン引数(argc, argv)
* atoi()による文字列変換

---

# テスト対策

## signal()

```c
signal(SIGUSR1, sig_handler);
```

受信設定

---

## kill()

```c
kill(pid, SIGUSR1);
```

送信

---

## atoi()

```c
atoi(argv[1]);
```

文字列→整数

---

## PID確認

```bash
ps u
```

---

## 覚えること

```text
signal()
→ シグナル受信設定

kill()
→ シグナル送信

SIGUSR1
→ ユーザ利用シグナル

ps u
→ PID確認

atoi()
→ 文字列を整数へ変換
```
