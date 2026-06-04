# 小問題 fork演習

## 目的

`fork()` を利用して、親プロセスと子プロセスを作成し、それぞれ異なる処理を実行する方法を理解する。

---

# 問題内容

`fork()` して次の動作をするプログラム `fork.c` を作成する。

* 子プロセスは `"child"` と表示して、2秒スリープする処理を10回繰り返す
* 親プロセスは `"parent"` と表示して、1秒スリープする処理を10回繰り返す
* `fork()` のエラーチェックも行う

---

# プログラム

ファイル名：`fork.c`

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid;
    int i;

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        for (i = 0; i < 10; i++) {
            printf("child\n");
            sleep(2);
        }
    } else {
        for (i = 0; i < 10; i++) {
            printf("parent\n");
            sleep(1);
        }
    }

    return 0;
}
```

---

# コンパイル

```bash
cc -o fork fork.c
```

---

# 実行

```bash
./fork
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

親プロセスは1秒ごとに表示し、子プロセスは2秒ごとに表示するため、表示順は完全には固定されない。

---

# 解説

## fork()

```c
pid = fork();
```

`fork()` は、現在実行中のプロセスをコピーして、新しい子プロセスを作成するシステムコールである。

---

## fork() の戻り値

| 戻り値        | 意味    |
| ---------- | ----- |
| `pid < 0`  | エラー   |
| `pid == 0` | 子プロセス |
| `pid > 0`  | 親プロセス |

---

## エラーチェック

```c
if (pid < 0) {
    perror("fork");
    return 1;
}
```

`fork()` に失敗すると `-1` が返る。

その場合、`perror("fork")` でエラー内容を表示して終了する。

---

## 子プロセスの処理

```c
if (pid == 0) {
    for (i = 0; i < 10; i++) {
        printf("child\n");
        sleep(2);
    }
}
```

`pid == 0` の場合は子プロセスである。

子プロセスでは、

* `"child"` を表示
* 2秒待つ

という処理を10回繰り返す。

---

## 親プロセスの処理

```c
else {
    for (i = 0; i < 10; i++) {
        printf("parent\n");
        sleep(1);
    }
}
```

`pid > 0` の場合は親プロセスである。

親プロセスでは、

* `"parent"` を表示
* 1秒待つ

という処理を10回繰り返す。

---

# 親プロセスと子プロセス

`fork()` の後は、親プロセスと子プロセスが並行して動作する。

ただし、変数はコピーされるため、親と子で同じ変数名を使っていても、実体は別物である。

---

# sleep()

```c
sleep(1);
```

指定した秒数だけ処理を停止する。

| 書き方        | 意味   |
| ---------- | ---- |
| `sleep(1)` | 1秒停止 |
| `sleep(2)` | 2秒停止 |

---

# 余力問題

## 内容

親プロセスが `n` 個の子プロセスを作成する。

* 親は `n` 個の子プロセスを作る
* n番目の子は、nの値を表示して1秒スリープする処理を10回繰り返す
* 親は子プロセスを作った後、`"parent"` と表示して1秒スリープする処理を10回繰り返す

---

## プログラム例

ファイル名：`fork_many.c`

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    int n = 2;
    int i, j;
    pid_t pid;

    for (i = 0; i < n; i++) {
        pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            for (j = 0; j < 10; j++) {
                printf("child %d\n", i);
                sleep(1);
            }
            return 0;
        }
    }

    for (i = 0; i < 10; i++) {
        printf("parent\n");
        sleep(1);
    }

    return 0;
}
```

---

# 余力問題のポイント

```c
for (i = 0; i < n; i++) {
    pid = fork();

    if (pid == 0) {
        ...
        return 0;
    }
}
```

子プロセス側で `return 0;` することが重要である。

もし `return 0;` を書かないと、子プロセスもさらに `fork()` を実行してしまい、想定以上にプロセスが増える。

---

# 学んだこと

* `fork()` によるプロセス生成
* 親プロセスと子プロセスの判定方法
* `fork()` の戻り値
* `perror()` によるエラー処理
* `sleep()` による待機
* プロセスが並行して動作すること
* 子プロセスを複数作成する方法

---

# テスト対策

## fork() の戻り値

```text
pid < 0  → エラー
pid == 0 → 子プロセス
pid > 0  → 親プロセス
```

---

## 基本形

```c
pid = fork();

if (pid < 0) {
    perror("fork");
} else if (pid == 0) {
    /* 子プロセス */
} else {
    /* 親プロセス */
}
```

---

## 暗記ポイント

```text
fork()
→ プロセスをコピーして子プロセスを作る

親プロセス
→ fork() の戻り値は子プロセスのPID

子プロセス
→ fork() の戻り値は0

失敗
→ -1
```
