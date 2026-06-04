# 小問題 GPIO読み取り演習

## 目的

Raspberry Pi の GPIO の値を読み取り、タクトスイッチが押されているか離されているかを確認する。

`~/pj/devices/gpio.c` をベースにして、GPIO の値を表示するプログラムを作成する。

---

# 問題内容

タクトスイッチは GPIO に接続されている。

そのため、GPIO の値を読み取ることで、スイッチが

* 押されている
* 離されている

のどちらの状態かを確認できる。

GPIO の値を読むには、配布された `gpio.c` に含まれている

```c
memread()
```

を使用する。

---

# GPIOとは

GPIO は

```text
General Purpose Input Output
```

の略である。

Raspberry Pi のピンを使って、外部機器と信号をやり取りするために使う。

---

# GPIOの値

GPIOの値はビットで管理されている。

例えば GPIO22 の状態を調べる場合、GPIO入力レベルレジスタの中の22番目のビットを見る。

---

# 基本プログラム例

ファイル名：`gpio.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define GPIO_PHY_BASEADDR 0x3F200000
#define GPIO_AREA_SIZE 4096
#define GPIO_GPLEV0 0x0034

volatile unsigned int *gpio;

unsigned int memread(unsigned int offset)
{
    return *(gpio + offset / 4);
}

int main()
{
    int fd;
    unsigned int value;

    fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    gpio = mmap(NULL,
                GPIO_AREA_SIZE,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                GPIO_PHY_BASEADDR);

    if (gpio == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    value = memread(GPIO_GPLEV0);

    printf("GPIO value = 0x%08x\n", value);
    printf("GPIO value = %u\n", value);

    munmap((void *)gpio, GPIO_AREA_SIZE);
    close(fd);

    return 0;
}
```

---

# コンパイル

```bash
cc gpio.c
```

実行ファイルは通常

```text
a.out
```

として作成される。

---

# 実行

GPIOを直接読むため、管理者権限が必要になる。

```bash
sudo ./a.out
```

---

# スイッチが押された時の値を確認する

スイッチが押された状態を確認するには、スイッチを押したまま実行する。

```bash
sudo ./a.out
```

---

# スイッチを離した時の値を確認する

スイッチを離した状態で実行する。

```bash
sudo ./a.out
```

---

# 解説

## /dev/mem

```c
open("/dev/mem", O_RDWR | O_SYNC);
```

物理メモリへアクセスするためのデバイスファイル。

GPIOレジスタを直接操作するために使用する。

---

## mmap()

```c
gpio = mmap(NULL,
            GPIO_AREA_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd,
            GPIO_PHY_BASEADDR);
```

GPIOの物理アドレスを、プログラムからアクセスできる仮想メモリに割り当てる。

---

## memread()

```c
value = memread(GPIO_GPLEV0);
```

指定したGPIOレジスタの値を読み取る。

---

## GPLEV0

```c
#define GPIO_GPLEV0 0x0034
```

GPIO 0〜31 の入力レベルを確認するレジスタである。

---

# GPIO22の確認例

タクトスイッチが GPIO22 に接続されている場合、22番目のビットを見る。

```c
if (value & (1 << 22)) {
    printf("GPIO22 = 1\n");
} else {
    printf("GPIO22 = 0\n");
}
```

---

## 完成版：GPIO22の状態表示

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define GPIO_PHY_BASEADDR 0x3F200000
#define GPIO_AREA_SIZE 4096
#define GPIO_GPLEV0 0x0034
#define GPIO22 22

volatile unsigned int *gpio;

unsigned int memread(unsigned int offset)
{
    return *(gpio + offset / 4);
}

int main()
{
    int fd;
    unsigned int value;

    fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    gpio = mmap(NULL,
                GPIO_AREA_SIZE,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                GPIO_PHY_BASEADDR);

    if (gpio == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    value = memread(GPIO_GPLEV0);

    printf("GPLEV0 = 0x%08x\n", value);

    if (value & (1 << GPIO22)) {
        printf("GPIO22 = 1\n");
        printf("switch state: high\n");
    } else {
        printf("GPIO22 = 0\n");
        printf("switch state: low\n");
    }

    munmap((void *)gpio, GPIO_AREA_SIZE);
    close(fd);

    return 0;
}
```

---

# ビット演算の意味

```c
value & (1 << GPIO22)
```

は、GPIO22 のビットだけを取り出して確認する処理である。

---

## 1 << GPIO22

```c
1 << 22
```

22番目のビットだけが1の値を作る。

---

## &

```c
value & (1 << 22)
```

GPIO22のビットが1なら真、0なら偽になる。

---

# 押した時と離した時の違い

回路によって、

```text
押した時 → 1
離した時 → 0
```

の場合もあれば、

```text
押した時 → 0
離した時 → 1
```

の場合もある。

そのため、実際に

* 押したまま実行
* 離したまま実行

して値を比較する。

---

# 使用した関数

| 関数        | 役割                |
| --------- | ----------------- |
| open()    | /dev/mem を開く      |
| mmap()    | 物理メモリを仮想メモリに割り当てる |
| memread() | GPIOレジスタを読む       |
| printf()  | 値を表示する            |
| munmap()  | メモリ割り当て解除         |
| close()   | ファイルを閉じる          |
| perror()  | エラー表示             |

---

# 学んだこと

* GPIOの値の読み取り
* mmapによるメモリマップドI/O
* /dev/mem の利用
* GPLEV0レジスタの意味
* ビット演算によるGPIOピンの状態確認
* タクトスイッチの押下状態の確認

---

# テスト対策

## GPLEV0

```text
GPIO 0〜31 の入力レベルを表すレジスタ
```

---

## memread()

```text
指定したGPIOレジスタの値を読む関数
```

---

## GPIO22確認

```c
value & (1 << 22)
```

---

## 押下確認方法

```text
スイッチを押したまま a.out を実行する
```

---

## 暗記ポイント

```text
/dev/mem
→ 物理メモリへアクセス

mmap
→ GPIOレジスタをプログラムから読めるようにする

GPLEV0
→ GPIOの入力値

1 << 22
→ GPIO22だけを見る

&演算
→ 対象ビットが1か確認
```
