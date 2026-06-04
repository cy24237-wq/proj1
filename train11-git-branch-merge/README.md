# 小問題 Gitブランチ・マージ演習

## 目的

Gitのブランチ機能を利用して、別々の作業内容を分岐させたあと、再び統合する方法を学ぶ。

また、同じファイルの同じ行を別々のブランチで修正した場合に発生するコンフリクトを確認し、手動で解決する方法を理解する。

---

# 問題内容

以下の作業を行う。

1. `develop` ブランチを作成する
2. `master` ブランチで `test.c` を修正してコミットする
3. `develop` ブランチに移動する
4. `develop` ブランチでも `test.c` の同じ行を別の内容に修正してコミットする
5. `develop` を `master` にマージする
6. コンフリクトを手動で解決する
7. 解決後にコミットする
8. `git log --graph` で枝分かれと統合を確認する

---

# ブランチとは

ブランチとは、Gitの履歴を枝分かれさせる機能である。

例えば、安定版を `master` に残したまま、新機能を `develop` で作ることができる。

---

# 手順

## 1. 現在のブランチ確認

```bash
git branch
```

現在いるブランチには `*` が付く。

例

```text
* master
```

---

## 2. developブランチ作成

```bash
git branch develop
```

この時点では、まだブランチが作成されただけで、移動はしていない。

---

## 3. masterブランチでtest.cを修正

`test.c` の同じ行をあとでdevelopでも修正する。

例

```c
printf("master branch\n");
```

---

## 4. masterでコミット

```bash
git add test.c
git commit -m "modify test.c on master"
```

---

## 5. developブランチへ移動

```bash
git checkout develop
```

または

```bash
git switch develop
```

---

## 6. developブランチでtest.cを修正

masterで修正した行と同じ行を、別の内容に修正する。

例

```c
printf("develop branch\n");
```

---

## 7. developでコミット

```bash
git add test.c
git commit -m "modify test.c on develop"
```

---

## 8. masterブランチへ戻る

```bash
git checkout master
```

または

```bash
git switch master
```

---

## 9. developをmasterへマージ

```bash
git merge develop
```

---

# コンフリクト

masterとdevelopで同じファイルの同じ行を別々に修正しているため、Gitはどちらを採用すればよいか判断できない。

そのため、以下のような警告が出る。

```text
CONFLICT (content): Merge conflict in test.c
Automatic merge failed; fix conflicts and then commit the result.
```

---

# コンフリクト時のtest.c

`test.c` を開くと、以下のような表示になる。

```c
<<<<<<< HEAD
printf("master branch\n");
=======
printf("develop branch\n");
>>>>>>> develop
```

---

# 記号の意味

## HEAD

```text
現在いるブランチ
```

今回の場合は `master`

---

## =======

```text
2つの変更内容の区切り
```

---

## >>>>>>> develop

```text
マージしようとしているブランチ
```

今回の場合は `develop`

---

# コンフリクト解決

どちらか片方を残すか、両方を組み合わせる。

例

```c
printf("merged master and develop\n");
```

このとき、以下の記号は必ず削除する。

```text
<<<<<<< HEAD
=======
>>>>>>> develop
```

---

# 解決後のコミット

```bash
git add test.c
git commit -m "resolve merge conflict"
```

---

# ログをグラフ表示

```bash
git log --graph --oneline --all
```

---

# 表示例

```text
*   abc1234 resolve merge conflict
|\
| * def5678 modify test.c on develop
* | ghi9012 modify test.c on master
|/
* xxx0000 first commit
```

---

# グラフの見方

```text
|\
```

ブランチが分かれたことを表す。

```text
|/
```

ブランチが統合されたことを表す。

---

# 余力問題：リモートリポジトリの変更を取り込む

## GitHub上でファイルを作成

GitHubのWebページで

```text
Add file
↓
Create new file
↓
Commit
```

を行う。

---

## リモートブランチを取得

```bash
git fetch origin master
```

これは、GitHub上の `master` ブランチをローカルに取得するコマンドである。

---

## ローカルブランチにマージ

```bash
git merge origin/master
```

取得したリモートブランチの内容を現在のローカルブランチに統合する。

---

## fetch + merge

以下の2つは、

```bash
git fetch origin master
git merge origin/master
```

---

次のコマンドと同じ意味である。

```bash
git pull origin master
```

---

## マージ後にpush

```bash
git push origin master
```

---

# 使用したGitコマンド

| コマンド                 | 役割             |
| -------------------- | -------------- |
| git branch           | ブランチ一覧表示       |
| git branch develop   | developブランチ作成  |
| git checkout develop | developブランチへ移動 |
| git switch develop   | developブランチへ移動 |
| git add              | 変更をインデックスへ追加   |
| git commit           | 変更を記録          |
| git merge            | ブランチを統合        |
| git log --graph      | 履歴をグラフ表示       |
| git fetch            | リモート情報を取得      |
| git pull             | fetch + merge  |
| git push             | リモートへ反映        |

---

# 学んだこと

* ブランチの作成方法
* ブランチの切り替え方法
* masterとdevelopの使い分け
* mergeの方法
* コンフリクトの発生理由
* コンフリクトの解決方法
* git log --graphによる履歴確認
* fetchとpullの違い

---

# テスト対策

## ブランチ作成

```bash
git branch develop
```

---

## ブランチ移動

```bash
git checkout develop
```

または

```bash
git switch develop
```

---

## マージ

```bash
git merge develop
```

---

## コンフリクト表示

```c
<<<<<<< HEAD
master側の内容
=======
develop側の内容
>>>>>>> develop
```

---

## コンフリクト解決

```text
不要な記号を削除し、正しい内容に修正する
```

---

## グラフ表示

```bash
git log --graph --oneline --all
```

---

# 暗記ポイント

```text
branch
→ ブランチ作成・確認

checkout / switch
→ ブランチ移動

merge
→ ブランチ統合

CONFLICT
→ 同じ行を別々に修正してGitが判断できない状態

fetch
→ リモート情報を取得

pull
→ fetch + merge

push
→ リモートへ反映
```
