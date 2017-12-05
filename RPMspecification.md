# 「ロボット プログラミング マネージャ」仕様

奥野 真之，升谷 保博  
大阪電気通信大学  
2017年12月4日  

## 必要なソフトウェア
  - OSはWindowsまたは，Linux
  - OpenRTM-aist-1.1.2
  - Choreonoid
  - Python 2.7.10 （2.7.11以上ではChoreonoidが起動しない）
  - rtctree
  - TkInter

## Choreonoid
  - 最新版ではなく，バージョン1.5.0でしか確認していない．
  - 実機に対応する動力学・形状のモデルをVRML（wrlファイル）で用意する必要がある．
  - RTミドルウェアに組み込むためにボディRTCを用意する．
    - 実機のコントローラの摸擬するために，ボディRTCのコントローラモジュールを動的ライブラリ（Windowsではdllファイル，Linuxではsoファイル）として用意する．
  - 各設定をまとめたプロジェクトファイル（拡張子cnoid）を用意する．
  - RPMは複数のプロジェクトファイルを切り替え可能．
  - Windowsでは，Choreonoid本体とボディRTCのコントローラモジュール（DLLファイル）をビルドしたVSのバージョンや構成が完全に一致していないと使えない．したがって，配布されているバイナリと利用者のVSのバージョンが一致しない場合は，独自にビルドする必要がある．RPM開発者らは，自分でVS2012でビルドしている．
  - シミュレーション実行で接続されているRTCをアクティベートする機能があるので，それを使う．
  - PythonのRTCでは，Choreonoidからアクティベートできないので（原因不明），RT System Editorまたはrtshellでアクティベート．

## インストール
  - Pythonで記述されているので，OpenRTMのインストールで必要なPython2.7.10．
  - 設定ファイルの読み込みに使うpyYAML
    - python2.7系ならバージョンは問わず
    - https://pyyaml.org/

- 機材の準備（一般的に）
  - ロボット実機
  - 実機PC
  - ユーザPC（複数でも可）
  - ネットワーク

## RTCの準備（一般的に）
  - 実機RTC
  - 支援RTC
  - ユーザRTC
  - Choreonoid RTC
    - ボディRTC
    - ボディRTCのコントローラモジュール

## ユーザPCにおける準備
  - RTC設定ファイル（rtc.conf）の設定（2種類の使い分け．```corba.nameservers:```と```naming.formats:```は必須．
    - 実機
      - corba.nameservers: リモートPCのIPアドレス
      - naming.formats: %h.host_cxt/%n.rtc
    - シミュレーション
      - corba.nameservers: localhost
      - naming.formats: %n.rtc
  - 必要なユーザRTCを実行できるようにする．
  - Choreonoidによるシミュレーションができるようにする．
  - ネームサーバ
     - 実機の場合は実機PCのものを使う．
     - シミュレーションの場合は，Choreonoidから起動されるcnoid-namingを使う．

## 設定ファイル
  - 概要はここで説明し，詳細は別ドキュメントで．
  - WIndowsとLinuxとの使い分け．

## リモートPCにおける実行
  - 実機との接続
  - ネームサーバ起動
  - リモートRTC間接続

## ユーザPCにおける実行
  - ネットワークインタフェースを一つだけにする．
  - ファイアウォールを切る．
  - ファイアウォールの機能を有するセキュリティソフトウェアを無効にする．
  - 実機PCとネットワークで接続する．
  - RPMを起動する．
  - 設定用のYAMLファイルを指定する．
  - ここから先はRPMのGUIで操作する．
  - 受講生視点の操作説明はREADME.mdに書く．

## RPMの内部動作
ボタンごとの機能説明．確認する項目で確認できなかった場合はそれを促すメッセージボックスを表示．
- シミュ
  - 実機でユーザRTCが起動していないことを確認
  - コレオノイドが起動していないことを確認
  - コンボボックスで指定した環境でコレオノイドと支援RTCを起動．
- ユーザRTC起動
  - 実機
    - コレオノイドが起動していないことを確認
    - pingコマンドでリモートPCの存在を確認
    - リモートPCがネームサーバを起動しているか確認
    - 設定ファイルに書かれているリモートRTCが起動しているか確認
    - リモートRTCがノンアクティブ状態を確認
    - 他ユーザがRTCを起動していないか確認
    - ユーザRTC起動
  - シミュレーション
    - コレオノイドが起動しているか確認
    - ユーザRTC起動
- アクティベート
  - 全RTCアクティベート
- ディアクティベート
  - 全RTCをディアクティベート
  - RTCが足りていない場合はそのRTCは無視し次のRTCをディアクティベート
- ユーザRTC終了
  - ユーザRTCがディアクティベートしていることを確認
  - ユーザRTCを終了
- 終了
  - ユーザRTCが終了していることを確認
  - 支援RTCとコレオノイドを終了