# 小問題 gnuplot演習

## 目的

`gnuplot` を利用して、rain.txt に保存されている降水量データを折れ線グラフとして表示する。

---

# データについて

rain.txt は

```text
年    1月    2月    3月 ... 12月    合計
```

の形式で保存されている。

例

```text
1900    52.1    31.2    ...
1901    48.5    22.7    ...
```

各列は TAB 区切りになっている。

---

# 問題①

## 1900～1999年の1月降水量を表示

### gnuplot起動

```bash
gnuplot
```

---

### コマンド

```gnuplot
plot "rain.txt" using 1:2 with lines
```

---

## 解説

### using 1:2

```text
1列目 → X軸
2列目 → Y軸
```

つまり

| 列   | 内容    |
| --- | ----- |
| 1列目 | 年     |
| 2列目 | 1月降水量 |

---

### with lines

```text
折れ線グラフ
```

として表示する。

---

## イメージ

```text
降水量
 ^
 |
 |      ／＼
 |    ／    ＼
 |  ／
 +---------------->
       年
```

---

# 問題②

## 1月と8月の降水量を同時表示

### コマンド

```gnuplot
plot "rain.txt" using 1:2 with lines title "January", \
     "rain.txt" using 1:9 with lines title "August"
```

---

## 解説

### 1月

```gnuplot
using 1:2
```

| 列 | 内容    |
| - | ----- |
| 1 | 年     |
| 2 | 1月降水量 |

---

### 8月

```gnuplot
using 1:9
```

| 列 | 内容    |
| - | ----- |
| 1 | 年     |
| 9 | 8月降水量 |

理由

```text
1列目 = 年

2列目 = 1月
3列目 = 2月
4列目 = 3月
5列目 = 4月
6列目 = 5月
7列目 = 6月
8列目 = 7月
9列目 = 8月
```

だから。

---

### title

```gnuplot
title "January"
```

凡例表示

---

## 表示例

```text
降水量
 ^
 |
 |  January
 |   ／＼
 |
 |      August
 |     ／￣＼
 |
 +---------------->
        年
```

---

# よく使う gnuplot コマンド

## sin(x)

```gnuplot
plot sin(x)
```

---

## x²

```gnuplot
plot x**2
```

---

## データファイル表示

```gnuplot
plot "rain.txt" using 1:2 with lines
```

---

## 複数グラフ

```gnuplot
plot "rain.txt" using 1:2 with lines,\
     "rain.txt" using 1:9 with lines
```

---

## グラフタイトル

```gnuplot
set title "Rainfall"
```

---

## X軸ラベル

```gnuplot
set xlabel "Year"
```

---

## Y軸ラベル

```gnuplot
set ylabel "Rainfall(mm)"
```

---

# テスト対策

## 1月降水量

```gnuplot
plot "rain.txt" using 1:2 with lines
```

---

## 1月と8月

```gnuplot
plot "rain.txt" using 1:2 with lines title "January", \
     "rain.txt" using 1:9 with lines title "August"
```

---

## 暗記ポイント

```text
using A:B

A列目 → X軸
B列目 → Y軸
```

```text
with lines
↓
折れ線グラフ
```

```text
title
↓
凡例表示
```

---

# 学んだこと

* gnuplot の基本操作
* plot コマンド
* using による列指定
* with lines による折れ線グラフ表示
* 複数系列の同時表示
* 凡例(title)の設定
