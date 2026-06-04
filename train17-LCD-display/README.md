# 小問題 LCD表示演習

## 目的

Raspberry Pi に接続された LCD を制御し、文字列を表示・消去する方法を学ぶ。

配布されている `lcd.c` には `main()` の中身がないため、指定された動作になるように完成させる。

---

# 問題内容

`~/pj/devices/lcd.c` を完成させる。

以下の動作を繰り返す。

1. `"hello"` を1秒表示する
2. 画面を消去する
3. `"world"` を1秒表示する
4. 画面を消去する
5. 以上を繰り返す

また、`lcd_cmdwrite()` と `lcd_datawrite()` はエラー時に `-1` を返すため、エラー処理を追加する。

---

# 基本方針

LCDに文字を表示するには、

```c
lcd_datawrite()
```

を使う。

LCDに命令を送るには、

```c
lcd_cmdwrite()
```

を使う。

画面を消去するには、Clear Display命令を送る。

---

# main関数の例

配布された `lcd.c` の関数名に合わせて、`main()` に以下を追加する。

```c
int main()
{
    int i;
    char *s1 = "hello";
    char *s2 = "world";

    while (1) {
        for (i = 0; s1[i] != '\0'; i++) {
            if (lcd_datawrite(s1[i]) < 0) {
                perror("lcd_datawrite");
                return 1;
            }
        }

        sleep(1);

        if (lcd_cmdwrite(0x01) < 0) {
            perror("lcd_cmdwrite");
            return 1;
        }

        sleep(1);

        for (i = 0; s2[i] != '\0'; i++) {
            if (lcd_datawrite(s2[i]) < 0) {
                perror("lcd_datawrite");
                return 1;
            }
        }

        sleep(1);

        if (lcd_cmdwrite(0x01) < 0) {
            perror("lcd_cmdwrite");
            return 1;
        }

        sleep(1);
    }

    return 0;
}
```

---

# 注意

配布された `lcd.c` に初期化関数がある場合は、最初に呼び出す必要がある。

例

```c
if (lcd_init() < 0) {
    perror("lcd_init");
    return 1;
}
```

この場合は `while(1)` の前に追加する。

---

# 文字表示の流れ

```text
文字列を1文字ずつ取り出す
↓
lcd_datawrite() でLCDに送る
↓
1秒待つ
↓
lcd_cmdwrite(0x01) で消去
```

---

# lcd_datawrite()

```c
lcd_datawrite('h');
```

LCDに文字データを書き込む関数。

文字列を直接渡すのではなく、1文字ずつ渡す。

---

# lcd_cmdwrite()

```c
lcd_cmdwrite(0x01);
```

LCDに命令を送る関数。

`0x01` は Clear Display の命令である。

---

# sleep()

```c
sleep(1);
```

1秒待つ。

---

# エラー処理

```c
if (lcd_datawrite(s1[i]) < 0) {
    perror("lcd_datawrite");
    return 1;
}
```

`lcd_datawrite()` が `-1` を返した場合、エラーとして処理する。

---

# 下の行に表示する余力問題

配布された `lcd.c` には `location()` という関数が用意されている。

第2引数が

```text
0 → 上の行
1 → 下の行
```

を表す。

---

## 例

```c
location(0, 0);
```

上の行に移動

```c
location(0, 1);
```

下の行に移動

---

## 上下2行表示の例

```c
location(0, 0);

for (i = 0; s1[i] != '\0'; i++) {
    if (lcd_datawrite(s1[i]) < 0) {
        perror("lcd_datawrite");
        return 1;
    }
}

location(0, 1);

for (i = 0; s2[i] != '\0'; i++) {
    if (lcd_datawrite(s2[i]) < 0) {
        perror("lcd_datawrite");
        return 1;
    }
}
```

---

# 余力問題1：Display ON/OFF

Clear Display は文字を消す命令である。

一方、Display ON/OFF は文字データを残したまま、画面表示だけを点灯・消灯する命令である。

---

## 例

```c
lcd_cmdwrite(0x0c);
```

Display ON

```c
lcd_cmdwrite(0x08);
```

Display OFF

---

## 0.5秒ごとに点灯・消灯

```c
lcd_cmdwrite(0x0c);
usleep(500000);

lcd_cmdwrite(0x08);
usleep(500000);
```

---

# コンパイル

環境によって異なるが、例として以下のように実行する。

```bash
cc -o lcd lcd.c
```

または、Makefileがある場合は

```bash
make
```

---

# 実行

```bash
./lcd
```

---

# 使用した関数

| 関数              | 役割          |
| --------------- | ----------- |
| lcd_datawrite() | LCDに文字を書き込む |
| lcd_cmdwrite()  | LCDに命令を書き込む |
| location()      | 表示位置を変更する   |
| sleep()         | 秒単位で待機      |
| usleep()        | マイクロ秒単位で待機  |
| perror()        | エラー内容を表示    |

---

# 学んだこと

* LCDへの文字表示
* LCDコマンドの送信
* Clear Display命令
* Display ON/OFF命令
* エラー処理
* `sleep()` による待機
* `location()` による表示行の変更

---

# テスト対策

## 文字表示

```c
lcd_datawrite('h');
```

---

## コマンド送信

```c
lcd_cmdwrite(0x01);
```

---

## 画面消去

```text
0x01
```

---

## 上の行

```c
location(0, 0);
```

---

## 下の行

```c
location(0, 1);
```

---

## Display ON

```c
lcd_cmdwrite(0x0c);
```

---

## Display OFF

```c
lcd_cmdwrite(0x08);
```

---

# 暗記ポイント

```text
lcd_datawrite()
→ 文字表示

lcd_cmdwrite()
→ 命令送信

0x01
→ 画面消去

location(x, 0)
→ 上の行

location(x, 1)
→ 下の行

sleep(1)
→ 1秒待つ
```
