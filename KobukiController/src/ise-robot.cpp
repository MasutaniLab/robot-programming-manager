//���V�X�e�������u���{�b�g�v���O���~���O�v
//2016/6/8 ���J �۔�
#include <cmath>
#include "ise-robot.h"
#include "util.h"

#include ISE_ROBOT_SOURCE

//�T�v�F���[���h���W�n���烍�[�J�����W�n�ւ̕ϊ�
//�����Fworld ���[���h���W�n�ɂ�������W�i���́j�Creference ���[�J�����W�n�̈ʒu�ƕ����i���́j
//�߂�l�F���[�J�����W�n�ɂ�������W
Orthogonal worldToLocal(const Orthogonal &world, const Orthogonal &reference)
{
  Orthogonal local;
  double cosq = cos(reference.q);
  double sinq = sin(reference.q);
  local.x =  cosq*(world.x-reference.x) + sinq*(world.y-reference.y);
  local.y = -sinq*(world.x-reference.x) + cosq*(world.y-reference.y);
  local.q = normalizeAngle(world.q-reference.q);
  return local;
}

//�T�v�F���[�J�����W�n���烏�[���h���W�n�ւ̕ϊ�
//�����Flocal ���[�J�����W�n�ɂ�������W�i���́j�Creference ���[�J�����W�n�̈ʒu�ƕ����i���́j
//�߂�l�F���[���h���W�n�ɂ�������W
Orthogonal localToWorld(const Orthogonal &local, const Orthogonal &reference)
{
  Orthogonal world;
  double cosq = cos(reference.q);
  double sinq = sin(reference.q);
  world.x = cosq*local.x - sinq*local.y + reference.x;
  world.y = sinq*local.x + cosq*local.y + reference.y;
  world.q = normalizeAngle(local.q+reference.q);
  return world;
}

//�T�v�F�������W�n����ɍ��W�n�ւ̕ϊ�
//�����Fo �������W�n�̍��W�i���́j
//�߂�l�F�ɍ��W�n�̍��W
Polar orthogonalToPolar(const Orthogonal &o)
{
  Polar p;
  p.r = sqrt(o.x*o.x+o.y*o.y);
  p.a = atan2(o.y, o.x);
  p.q = o.q;
  return p;
}

//�T�v�F�ɍ��W�n���璼�����W�n�ւ̕ϊ�
//�����Fp �ɍ��W�n�̍��W�i���́j
//�߂�l�F�������W�n�̍��W
Orthogonal polarToOrthogonal(const Polar &p)
{
  Orthogonal o;
  o.x = p.r*cos(p.a);
  o.y = p.r*sin(p.a);
  o.q = p.q;
  return o;
}