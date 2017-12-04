//情報システム実験「ロボットプログラミング」
//2016/6/8 升谷 保博
#include <cmath>
#include "ise-robot.h"
#include "util.h"

#include ISE_ROBOT_SOURCE

//概要：ワールド座標系からローカル座標系への変換
//引数：world ワールド座標系における座標（入力），reference ローカル座標系の位置と方向（入力）
//戻り値：ローカル座標系における座標
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

//概要：ローカル座標系からワールド座標系への変換
//引数：local ローカル座標系における座標（入力），reference ローカル座標系の位置と方向（入力）
//戻り値：ワールド座標系における座標
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

//概要：直交座標系から極座標系への変換
//引数：o 直交座標系の座標（入力）
//戻り値：極座標系の座標
Polar orthogonalToPolar(const Orthogonal &o)
{
  Polar p;
  p.r = sqrt(o.x*o.x+o.y*o.y);
  p.a = atan2(o.y, o.x);
  p.q = o.q;
  return p;
}

//概要：極座標系から直交座標系への変換
//引数：p 極座標系の座標（入力）
//戻り値：直交座標系の座標
Orthogonal polarToOrthogonal(const Polar &p)
{
  Orthogonal o;
  o.x = p.r*cos(p.a);
  o.y = p.r*sin(p.a);
  o.q = p.q;
  return o;
}