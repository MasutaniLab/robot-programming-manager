# 実機とシミュレータを併用するロボットプログラミング教材のための支援ツール

奥野 真之，升谷 保博  
大阪電気通信大学  
2017年12月4日  

<p>
<img src="images/system.jpg" title="システム概要図"  width="50%" height="50%" align="right">

</p> 

## 概要
ロボットプログラミングの学習では実機(車輪ロボット，ロボットアーム等)とシミュレータを切り替えることのできる教材が有用である．大阪電気通信大学では，RTミドルウェアとChoreonoidを利用してそのような教材を構成し授業を行っている．しかし，それらの標準のソフトウェアだけでは，操作が煩雑で学習の妨げになりやすく，実機を複数人で共用する場合にトラブルが起こりがちであった．そこでそのトラブルを解消する支援ツール，ロボットプログラミングマネージャ(RPM)を作成した．

## 特徴
- rtctreeとTkinterを利用したGUIの支援ツールを開発し授業で活用した．
- OpenRTMでインストールするファイルだけで使用できる．
- 設定ファイルを書き換えることによって様々な実機とシミュレータの組み合わせに利用できるように汎用化したツールである．

## 操作の概要
OpenRTMに必要な操作，コンポーネント起動，接続，アクティベート，ディアクティベート，終了をする．ユーザは基本的にボタンを上から下に押していくだけでそれを行っている．図のように実機の場合はロボット1に対しユーザ多を，シミュレーションの場合は自分のPC(ローカルPC)のみ，の環境を想定している．（下図）

<p><img src="images/rpm.jpg" title="RPM"  width="50%" height="50%" align="right"></p> 

## 必要なソフトウェア
- python2.7.10
- Tkinter,rtctree
- pyYAML(python 2.7に準ずるならバージョンを問わず)

## 前提環境
- Windows
- Linux

## 準備(教材を準備する側)
- RTCの準備はもちろん，設定ファイルやconfファイルも設定しないといけない．
- 各コンポーネントをReleaseでビルド
- ボディRTCのコントローラモジュール
  - Windows: Releaseでビルドし，その後必ずINSTALLをビルド．
  - Linux: makeした後make install 
- 設定ファイルの記述
  - 詳細はconfig.mdや同リポジトリのyamlファイルを参照
詳細は仕様書，事例1，事例2のドキュメント参照



## 準備(受講生側)
- インストール
  - OpenRTM
  - Python2.7.10
  - pyYAML
- 全RTCの準備
  - CMake,make


## 実機
- 利用するPCは2台．
  - 実機側（リモートPC）： 実機を動かすためのPC．実機用のRTC(リモートRTC)はここで実行．
  - ユーザ側（ローカルPC）： ユーザのRTC(ユーザRTC)をここで実行．

### リモートPC準備

- IPアドレスを固定
- ネームサーバ起動
- リモートRTC起動
    - リモートRTC間の接続はRT System Editor等で済ませる

### 操作
- ローカルPCでRPM起動
- "ユーザRTC起動"で設定ファイルに書かれてる```common```の```command```が立ち上がり，問題がなければ接続完了と表示される
- "アクティベート"，"ディアクティベート"で全RTCがアクティベート，ディアクティベートになる
- "ユーザRTC終了"でユーザRTCが閉じる

## シミュレーション
- 利用するPCは1台
  - ローカルPCでユーザRTCとサポートRTC（実機ではリモートRTC）を一括操作

### 操作
- コンボボックスで設定ファイルに書かれてるプロジェクトファイルを切り替えれる
- "シミュ"でChoreonoidとサポートRTCを起動
- "ユーザRTC起動"で設定ファイルに書かれてる```common```の```command```が立ち上がり，問題がなければ接続完了と表示される
- アクティベート，ディアクティベートはChoreonoidに任せる
- "ユーザRTC終了"でユーザRTCが閉じる


# 仕様書 
- https://github.com/MasutaniLab/robot-programming-manager/blob/master/RPMspecification.md


# 設定ファイルの仕様
- https://github.com/MasutaniLab/robot-programming-manager/blob/master/config.md


# 事例1
- https://github.com/MasutaniLab/robot-programming-manager/blob/master/sample1.md


# 事例2
- https://github.com/MasutaniLab/robot-programming-manager/blob/master/sample2.md
