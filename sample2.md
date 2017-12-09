# 事例2: ロボットアームCRANE+の制御

奥野 真之，升谷 保博  
大阪電気通信大学  
2017年12月4日  

アールティ社のロボットアームCRANE+を使用し，手先座標で位置決めをする教材である．手先座標系で作業をプログラムすることを想定しているが，ここでは，単純に手先座標を与えているだけである．

<p><img src="images/CRANE_.jpg" title="CRANE+"  width="50%" height="50%" align="right"/></p> 
<br>
<p><img src="images/cho2_.jpg" title="choreonoid"  width="30%" height="30%" align="right"/></p> 

※ WindowsとLinuxのローカルPCは動作確認したが，リモートPCはWindowsのみ

### 準備物
インストール方法は各サイトを参照．
- DynamixelSDK
  - https://github.com/ROBOTIS-GIT/DynamixelSDK
### 設定ファイル

事例2で使う設定ファイル．

- Windows
  - https://github.com/MasutaniLab/robot-programming-manager/blob/master/sample2Win.yaml
- Linux 
  - https://github.com/MasutaniLab/robot-programming-manager/blob/master/sample2Linux.yaml


## RTC接続図
左が実機，右がシミュレーション
<p><img src="images/crane_real.jpg" title="実機のRTC接続図"  width="40%" height="40%"/> <img src="images/crane_sim.jpg" title="シミュレーションのRTC接続図"  width="40%" height="40%"/></p> 

### リモートRTC
- CraneplusWrapper0.rtc  
  - 順，逆運動学を解くRTC
- Dynamixel0.rtc
  - Dynamixelを動かすRTC
  
### シミュレーション

- ロボットアームのモデル
  - RPMのリポジトリのサブモジュールになっている．
  - https://github.com/MasutaniLab/CraneplusForChoreonoid
  - CraneplusForChoreonoidに以下がすべて含まれている．
    - ロボットのモデル（VRMLファイル，拡張子wrl）
    - Choreonoidのプロジェクトファイル（拡張子cnoid）
    - ボディRTCのコントローラモジュール（DynamixelSim，cmakeしてビルドする）

- シミュレーションの設定
  - Choreonoidのプロジェクトファイル
  - CraneplusForChoreonoid/craneplus.cnoid

- 支援RTC
  - CraneplusWrapper0.rtc  
- Choreonoid RTC
  - CRANE.rtc
    - BodyRTC
  - DynamixelSim0.rtc
    - ChoreonoidのBodyRTCのコントローラモジュール
    - CraneplusForChoreonoid/DynamixelSim ディレクトリ以下
    - cmakeで生成
  
- ChoreonoidのBodyRTCの設定ファイル
  - CraneplusForChoreonoid/CRANE.conf  
  
### ユーザRTC

<p><img src="images/command.jpg" title="TkSlider"  width="30%" height="30%"/> <img src="images/monitor.jpg" title="TkMonitorSlider"  width="30%" height="30%"/></p> 

- TkSlider
  - 手先座標を指定
- TkMonitorSlider
  - 手先座標を表示

