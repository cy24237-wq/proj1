# 小問題2 Shellスクリプト演習

## 目的

Shellスクリプトを作成し、複数のコマンドをまとめて実行する方法と、コマンドライン引数の利用方法を理解する。

---

# 問題1：llコマンドを作成する

## 要求

以下を順番に表示するコマンド `ll` を作成する。

* 現在時刻
* 現在のディレクトリ
* 詳細なファイル一覧

---

## プログラム

ファイル名：`ll`

```bash
#!/bin/bash

date
pwd
ls -l
```

---

## 実行権限付与

```bash
chmod u+x ll
```

---

## 実行

```bash
./ll
```

または

```bash
ll
```

---

## 解説

### date

現在時刻を表示

```bash
date
```

出力例

```text
Thu Jun 4 12:30:00 JST 2026
```

---

### pwd

現在いるディレクトリを表示

```bash
pwd
```

出力例

```text
/home/haru
```

---

### ls -l

詳細なファイル一覧を表示

```bash
ls -l
```

出力例

```text
-rw-r--r-- 1 haru haru 120 Jun 4 test.c
```

---

## 学んだこと

* Shellスクリプトの作成方法
* 複数コマンドの連続実行
* dateコマンド
* pwdコマンド
* ls -lコマンド
* chmodによる実行権限設定

---

# 問題2：fコマンドを作成する

## 要求

通常は

```bash
find . -name ファイル名 -print
```

と入力する必要がある。

これを

```bash
f ファイル名
```

だけで検索できるようにする。

---

## プログラム

ファイル名：`f`

```bash
#!/bin/bash

find . -name "$1" -print
```

---

## 実行権限付与

```bash
chmod u+x f
```

---

## 実行例

```bash
f test.c
```

実際には

```bash
find . -name test.c -print
```

が実行される。

---

## 解説

### find

ファイル検索コマンド

```bash
find . -name test.c -print
```

意味

| 部分     | 意味          |
| ------ | ----------- |
| .      | 現在のディレクトリ以下 |
| -name  | 名前指定        |
| test.c | 探したいファイル    |
| -print | 結果表示        |

---

### $1

特殊変数

```bash
$1
```

は

```text
第1引数
```

を表す。

---

### 例

実行

```bash
f report.txt
```

↓

スクリプト内部

```bash
find . -name "$1" -print
```

↓

実際

```bash
find . -name report.txt -print
```

として実行される。

---

## 引数一覧

| 変数 | 意味   |
| -- | ---- |
| $1 | 第1引数 |
| $2 | 第2引数 |
| $3 | 第3引数 |

---

## 学んだこと

* Shellスクリプト作成
* findコマンド
* コマンドライン引数
* 特殊変数 `$1`
* chmodによる実行権限付与

---

# テスト対策ポイント

### ll

```bash
#!/bin/bash

date
pwd
ls -l
```

---

### f

```bash
#!/bin/bash

find . -name "$1" -print
```

---

### 暗記

```text
date → 現在時刻

pwd → 現在位置

ls -l → 詳細表示

find → ファイル検索

$1 → 第1引数

chmod u+x → 実行権限付与
```
