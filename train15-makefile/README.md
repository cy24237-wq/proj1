# 小問題 Makefile演習

## 目的

Makefileを利用して、

* クロスコンパイル
* 自動ビルド
* clean処理
* 複数プログラムの同時コンパイル

を行う方法を学ぶ。

---

# make -n とは

Makefileが正しいか確認するためのコマンドである。

```bash
make -n
```

---

## 役割

実際にはコマンドを実行せず、

```text
実行される予定のコマンド
```

だけを表示する。

---

## 利点

誤った Makefile により

```text
rm *
```

のような危険なコマンドが実行される前に確認できる。

---

# 問題1

## クロスコンパイル用Makefile

---

### main.c

```c
#include <stdio.h>

int main()
{
    printf("hello\n");
    return 0;
}
```

---

### Makefile

```make
CC=arm-linux-gnueabihf-gcc

m: main.o
	$(CC) -o m main.o

main.o: main.c
	$(CC) -c main.c
```

---

## 実行

```bash
make
```

---

## 動作

```text
main.c
↓
main.o
↓
m
```

を作成する。

---

# 解説

## 変数

```make
CC=arm-linux-gnueabihf-gcc
```

クロスコンパイラを指定する。

---

## リンク

```make
m: main.o
	$(CC) -o m main.o
```

実行ファイルを作成する。

---

## コンパイル

```make
main.o: main.c
	$(CC) -c main.c
```

オブジェクトファイルを作成する。

---

# 問題2

## make clean

---

### Makefile

```make
CC=arm-linux-gnueabihf-gcc

m: main.o
	$(CC) -o m main.o

main.o: main.c
	$(CC) -c main.c

clean:
	rm -f m *.o
```

---

## 実行

```bash
make clean
```

---

## 結果

削除されるファイル

```text
m
main.o
```

---

# -f の意味

```bash
rm -f
```

存在しなくてもエラーを出さない。

---

# PHONY指定（推奨）

```make
.PHONY: clean
```

---

### 完全版

```make
CC=arm-linux-gnueabihf-gcc

m: main.o
	$(CC) -o m main.o

main.o: main.c
	$(CC) -c main.c

.PHONY: clean

clean:
	rm -f m *.o
```

---

# 問題3

## sig.c と rec.c を同時ビルド

---

### Makefile

```make
target: sig rec

sig: sig.o
	cc -o sig sig.o

rec: rec.o
	cc -o rec rec.o

sig.o: sig.c
	cc -c sig.c

rec.o: rec.c
	cc -c rec.c

clean:
	rm -f sig rec *.o
```

---

## 実行

```bash
make
```

---

## 動作

```text
sig.c
↓
sig.o
↓
sig

rec.c
↓
rec.o
↓
rec
```

を生成する。

---

# なぜ target が必要なのか

```make
target: sig rec
```

は

```text
sig と rec が存在する状態
```

を目標とする。

---

## target 自体は作られない

```make
target:
```

に実行コマンドが無いので、

```text
target
```

というファイルは生成されない。

---

## しかし

```text
targetを作るには
sig と rec が必要
```

なので、

```text
sig
rec
```

が毎回チェックされる。

---

# make sig rec

別の方法として

```bash
make sig rec
```

も利用できる。

---

# Makefileの基本構造

```make
ターゲット: 依存ファイル
	実行コマンド
```

---

## 例

```make
main.o: main.c
	cc -c main.c
```

意味

```text
main.c が更新されたら
main.o を作り直す
```

---

# よく使うコマンド

## 通常ビルド

```bash
make
```

---

## 実行予定確認

```bash
make -n
```

---

## 削除

```bash
make clean
```

---

## 特定ターゲット

```bash
make sig
```

---

## 全ターゲット

```bash
make target
```

---

# 学んだこと

* Makefileの基本構造
* クロスコンパイル
* make -n の使い方
* make clean の作成
* 複数ターゲットの管理
* target と依存関係
* PHONY の意味

---

# テスト対策

## Makefile基本形

```make
target: dependency
	command
```

---

## 実行予定確認

```bash
make -n
```

---

## clean

```make
clean:
	rm -f m *.o
```

---

## クロスコンパイル

```make
CC=arm-linux-gnueabihf-gcc
```

---

## 複数プログラム

```make
target: sig rec
```

---

## 暗記ポイント

```text
make
→ ビルド

make -n
→ 実行内容確認

make clean
→ 不要ファイル削除

CC=
→ コンパイラ指定

target: sig rec
→ sig と rec を作る
```
