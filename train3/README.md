# 小問題3 ファイル記述子演習

## 目的

Linuxのシステムコール

* open()
* read()
* write()
* close()

を利用し、ファイルからデータを読み込んで表示する方法を理解する。

また、

* ファイル記述子
* 標準入力
* 標準出力
* 標準エラー出力
* perror()

について理解する。

---

# 元のプログラム

```c
#include <fcntl.h>

int main() {
    char buff[5];
    int fd;

    fd = open("test.txt", O_RDONLY);
    read(fd, buff, 5);
    write(1, buff, 5);
    close(fd);
}
```

---

# 課題内容

次の2点を修正する。

## 修正①

5文字ではなく

```text
10文字
```

読み込むようにする。

---

## 修正②

エラーチェックを追加する。

---

# 完成プログラム

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buff[10];
    int fd;

    fd = open("test.txt", O_RDONLY);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (read(fd, buff, 10) < 0) {
        perror("read");
        return 1;
    }

    if (write(1, buff, 10) < 0) {
        perror("write");
        return 1;
    }

    if (close(fd) < 0) {
        perror("close");
        return 1;
    }

    return 0;
}
```

---

# 解説

## open()

ファイルを開く。

```c
fd = open("test.txt", O_RDONLY);
```

### O_RDONLY

```text
Read Only
```

読み込み専用

---

### 成功

```c
fd >= 0
```

ファイル記述子が返る

例

```text
3
4
5
```

---

### 失敗

```c
fd == -1
```

---

## read()

ファイルからデータを読む。

```c
read(fd, buff, 10);
```

意味

```text
fdから
10バイト読み込み
buffへ格納
```

---

### 成功

読み込んだバイト数を返す

例

```text
10
```

---

### 失敗

```text
-1
```

---

## write()

データを出力する。

```c
write(1, buff, 10);
```

---

### 1とは

ファイル記述子1

つまり

```text
標準出力
```

画面へ表示

---

## close()

ファイルを閉じる。

```c
close(fd);
```

開いたファイルは必ず閉じる。

---

# perror()

## 役割

エラー内容を表示

```c
perror("open");
```

---

## 例

test.txtが存在しない

```text
open: No such file or directory
```

表示

---

# ファイル記述子

Linuxでは

ファイル

キーボード

画面

もすべて番号で管理する。

---

## 標準入力

```text
0
```

キーボード

---

## 標準出力

```text
1
```

画面

---

## 標準エラー出力

```text
2
```

エラー表示

---

# 例

## キーボードから読む

```c
read(0, buff, 10);
```

---

## 画面へ出力

```c
write(1, buff, 10);
```

---

## エラー出力へ表示

```c
write(2, buff, 10);
```

---

# 余力問題①

## 標準エラー出力

```c
write(2, buff, 10);
```

使用

---

### リダイレクト確認

```bash
./a.out > out
```

すると

標準出力だけ

```text
out
```

へ保存

---

### エラー出力

```text
画面
```

に残る

---

# 余力問題②

## キーボード入力をファイルへ保存

### 読み込み

```c
read(0, buff, 10);
```

---

### 書き込み

```c
write(fd, buff, 10);
```

---

# テスト対策

## open

```text
ファイルを開く
```

---

## read

```text
読み込む
```

---

## write

```text
書き込む
```

---

## close

```text
閉じる
```

---

## perror

```text
エラー内容表示
```

---

## ファイル記述子

```text
0 → 標準入力

1 → 標準出力

2 → 標準エラー出力
```

---

# 暗記ポイント

```c
fd = open("test.txt", O_RDONLY);

read(fd, buff, 10);

write(1, buff, 10);

close(fd);
```

```text
open
↓
read
↓
write
↓
close
```

これがLinuxのファイル操作の基本的な流れである。
