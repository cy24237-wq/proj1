# 小問題 クロスコンパイル演習

## 目的

同じプログラムを

* ホスト環境
* ターゲット環境

で実行し、実行速度の違いを確認する。

また、

* クロスコンパイル
* timeコマンド
* clock_gettime()

について理解する。

---

# 問題内容

1～10億までの整数を加算するプログラムを作成する。

その後、

1. ホストで実行時間を測定する
2. クロスコンパイルする
3. ターゲットで実行時間を測定する
4. 実行時間を比較する

---

# プログラム

ファイル名：`main.c`

```c
#include <stdio.h>

int main()
{
    long long sum = 0;
    long long i;

    for (i = 1; i <= 1000000000LL; i++) {
        sum += i;
    }

    printf("sum = %lld\n", sum);

    return 0;
}
```

---

# ホストでコンパイル

```bash
cc -o main main.c
```

---

# ホストで実行

```bash
time ./main
```

---

# 実行例

```text
sum = 500000000500000000

real    0m2.3s
user    0m2.3s
sys     0m0.0s
```

---

# timeコマンド

## 書式

```bash
time 実行ファイル
```

---

## real

```text
実際に経過した時間
```

壁時計時間（Wall Clock Time）

---

## user

```text
ユーザプログラムがCPUを使用した時間
```

---

## sys

```text
OSカーネルが使用した時間
```

---

# クロスコンパイル

## コンパイル

例

```bash
arm-linux-gnueabihf-gcc -o main main.c
```

環境によってコマンド名は異なる。

---

# ターゲットへ転送

例

```bash
scp main pi@ターゲット:/home/pi
```

---

# ターゲットで実行

```bash
time ./main
```

---

# 結果比較

一般的には

```text
ホストPC
↓
高速

ターゲット
↓
低速
```

となる。

理由は

* CPU性能
* クロック周波数
* キャッシュサイズ

が異なるためである。

---

# ホスト用実行ファイルをターゲットで実行した場合

ホストで

```bash
cc -o main main.c
```

して作成した実行ファイルは

```text
x86_64
```

向けである。

---

ターゲットが

```text
ARM
```

の場合、

```bash
./main
```

を実行すると

```text
cannot execute binary file
```

や

```text
Exec format error
```

などのエラーになる。

---

# なぜ実行できないのか

CPUアーキテクチャが異なるため。

例

```text
ホスト
x86_64

ターゲット
ARM
```

---

# 余力問題

## プログラム内で実行時間測定

### 開始時刻

```c
clock_gettime(CLOCK_REALTIME, &start);
```

---

### 終了時刻

```c
clock_gettime(CLOCK_REALTIME, &end);
```

---

### 経過時間計算

```c
elapsed =
(end.tv_sec - start.tv_sec)
+
(end.tv_nsec - start.tv_nsec) / 1e9;
```

---

# real時間測定プログラム

```c
#include <stdio.h>
#include <time.h>

int main()
{
    struct timespec start, end;
    double elapsed;
    long long sum = 0;
    long long i;

    clock_gettime(CLOCK_REALTIME, &start);

    for(i = 1; i <= 1000000000LL; i++) {
        sum += i;
    }

    clock_gettime(CLOCK_REALTIME, &end);

    elapsed =
        (end.tv_sec - start.tv_sec)
        + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("sum = %lld\n", sum);
    printf("real = %f sec\n", elapsed);

    return 0;
}
```

---

# user時間測定

`CLOCK_REALTIME`

ではなく

```c
CLOCK_PROCESS_CPUTIME_ID
```

を利用する。

---

## 例

```c
clock_gettime(
    CLOCK_PROCESS_CPUTIME_ID,
    &start
);
```

---

## 意味

```text
CPUが実際に使用した時間
```

を測定する。

これは

```text
timeコマンドのuser時間
```

に近い値になる。

---

# 使用したコマンド

| コマンド          | 役割     |
| ------------- | ------ |
| cc            | コンパイル  |
| time          | 実行時間測定 |
| gcc           | コンパイル  |
| scp           | ファイル転送 |
| clock_gettime | 時刻取得   |

---

# 学んだこと

* クロスコンパイル
* ホストとターゲットの違い
* 実行時間測定
* timeコマンド
* real / user / sys の意味
* CPUアーキテクチャの違い
* clock_gettimeによる時間測定

---

# テスト対策

## 実行時間測定

```bash
time ./main
```

---

## real

```text
実際の経過時間
```

---

## user

```text
CPU使用時間
```

---

## sys

```text
カーネル使用時間
```

---

## real時間測定

```c
clock_gettime(CLOCK_REALTIME, &ts);
```

---

## user時間測定

```c
clock_gettime(
CLOCK_PROCESS_CPUTIME_ID,
&ts);
```

---

## クロスコンパイルの目的

```text
別CPU向けの実行ファイルを作ること
```

---

## 覚えること

```text
time
→ 実行時間測定

real
→ 実時間

user
→ CPU時間

CLOCK_REALTIME
→ 実時間

CLOCK_PROCESS_CPUTIME_ID
→ CPU時間

x86の実行ファイルは
ARMでは実行できない
```
