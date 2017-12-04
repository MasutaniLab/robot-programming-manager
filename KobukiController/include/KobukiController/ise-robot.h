//情報システム実験「ロボットプログラミング」
//2016/6/8 升谷 保博
#pragma once
//#define ISE_ROBOT_SOURCE "example0.cpp"
//#define ISE_ROBOT_SOURCE "example1.cpp"
#define ISE_ROBOT_SOURCE "example2.cpp"
//#define ISE_ROBOT_SOURCE "kadai1.cpp"
//#define ISE_ROBOT_SOURCE "kadai2.cpp"

struct Orthogonal {
  double x; //x座標 [m]
  double y; //y座標 [m]
  double q; //x軸と成す角度 [rad]
};

struct Polar {
  double r; //動径（距離） [m]
  double a; //偏角（角度） [rad]
  double q; //x軸と成す角度 [rad]
};

struct Velocity {
  double v; //並進速度 [m/s]
  double w; //回転速度 [rad/s]
};

struct Bumper {
  bool right;  //右バンパが反応しているか？
  bool center; //中央バンパが反応しているか？
  bool left;   //左バンパが反応しているか？
};

Orthogonal worldToLocal(const Orthogonal &world, const Orthogonal &reference);
Orthogonal localToWorld(const Orthogonal &local, const Orthogonal &reference);
Polar orthogonalToPolar(const Orthogonal &o);
Orthogonal polarToOrthogonal(const Polar &p);

void setup();
Velocity loop(const Orthogonal &pos, const Bumper &bum, int key);


