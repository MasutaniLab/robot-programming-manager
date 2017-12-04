//���V�X�e�������u���{�b�g�v���O���~���O�v
//2016/6/8 ���J �۔�
#pragma once
//#define ISE_ROBOT_SOURCE "example0.cpp"
//#define ISE_ROBOT_SOURCE "example1.cpp"
#define ISE_ROBOT_SOURCE "example2.cpp"
//#define ISE_ROBOT_SOURCE "kadai1.cpp"
//#define ISE_ROBOT_SOURCE "kadai2.cpp"

struct Orthogonal {
  double x; //x���W [m]
  double y; //y���W [m]
  double q; //x���Ɛ����p�x [rad]
};

struct Polar {
  double r; //���a�i�����j [m]
  double a; //�Ίp�i�p�x�j [rad]
  double q; //x���Ɛ����p�x [rad]
};

struct Velocity {
  double v; //���i���x [m/s]
  double w; //��]���x [rad/s]
};

struct Bumper {
  bool right;  //�E�o���p���������Ă��邩�H
  bool center; //�����o���p���������Ă��邩�H
  bool left;   //���o���p���������Ă��邩�H
};

Orthogonal worldToLocal(const Orthogonal &world, const Orthogonal &reference);
Orthogonal localToWorld(const Orthogonal &local, const Orthogonal &reference);
Polar orthogonalToPolar(const Orthogonal &o);
Orthogonal polarToOrthogonal(const Polar &p);

void setup();
Velocity loop(const Orthogonal &pos, const Bumper &bum, int key);


