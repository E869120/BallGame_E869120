# Valuable Balls
ボールを集める楽しいゲームです．プレイ画面の例は以下のようになります．

![16](https://user-images.githubusercontent.com/30901380/130218199-b6adcc0d-df5c-4c63-9afb-a6f9a27862cd.jpg)

<br />

## レポジトリの概要
OpenSiv3D を使っていますが，以下の 2 つのファイルがあれば動作します．

* **App/E869120-COLLECTING-BALLS.exe**：ゲームアプリ本体
* **App/ranking.txt**：ゲームの順位表

なお，Main.cpp はゲーム作成に使用したソースコードです．

<br />

## ゲームのルールについて
以下のようなルールとなっています．

* 上下左右からボールが降ってきます．
* 60 本の棒の状態（ON／OFF）を操作することで，できるだけ多くのボールを集めます．
* ボールは状態が ON の棒を通り抜けられませんが，状態が OFF の棒は通り抜けられます．
* ボールによって得点が異なり，青色は 5 点，黄色は 2 点，赤色は -2 点です．
* 制限時間は 150 秒です．できるだけ多くの得点を取ってください．

<br />

## 得点の目安
以下の表の通りになります．初心者は 50 点を目指してみると良いかもしれません．

なお，200 点以上になると最高ランクが獲得できます．

| 点数 | ランク | 説明 |
|:---:|:---:|:---:|
| 110～ | ??? | この上にも 10 種類のランクがあります．|
| 105～109 | SSS | 上位 15% に入るでしょう． |
| 100～104 | SS | |
| 95～99 | S | 上位 25% に入るでしょう． |
| 90～94 | A+ | |
| 85～89 | A | |
| 80～84 | A- | 優です．この点数を超えれば上手いです． |
| 75～79 | B+ | |
| 70～74 | B | |
| 65～69 | B- | 良です．おそらく一般参加者の平均点程度でしょう． |
| 60～64 | C+ | |
| 50～59 | C | 可です．初心者の目標点です． |
| 0～49 | F | 不可です．もう一度頑張りましょう． |
