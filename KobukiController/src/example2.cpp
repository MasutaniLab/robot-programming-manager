//情報システム実験「ロボットプログラミング」
//例プログラム2: 目標への移動＋障害物回避
//2016/6/29 升谷 保博
#include "ise-robot.h"
#include <iostream>
#include <cmath>
#include "draw.h"
#include "util.h"
using namespace std;

const int NumTarget = 2;
const Orthogonal targetList[NumTarget] = {{1, 3.5, 0}, {2, 0, 0}};
int iTarget;
bool pause;
double startTime, goalTime;
enum State {Move, AvoidBack, AvoidMove, Goal};
State state;
double contactTime;
Orthogonal subTarget;

//概要：行動決定のための初期設定
//引数：なし
//戻り値：なし
void setup()
{
  iTarget = 0;
  pause = true;
  startTime = goalTime = 0;
  state = Move;
  contactTime = 0;
  subTarget = targetList[0];
}

//概要：行動決定のための毎回の処理
//引数：pos 現在位置，bum バンパセンサの状態，key キー入力
//戻り値：速度指令
Velocity loop(const Orthogonal &pos, const Bumper &bum, int key)
{
  Velocity vel;
  double currentTime = getTime(); //現在の経過時間を記憶
  if (key >= 0) { //キーが押されていれば
    if (pause && key == 'r') { //停止中で「r」であれば
      pause = false;
      if (startTime == 0) { //初めてであれば
        startTime = currentTime;
      }
    } else { //動作中か「r」でなければ
      pause = true;
    }
  }
  if (pause) { //停止中であれば
    drawString(0,2,36,255,0,0,"Press 'r' key to start");
    vel.v = 0;
    vel.w = 0;
    return vel;
  }
  //目標に十字を描く
  drawCross(targetList[iTarget].x, targetList[iTarget].y, 255, 0, 0, 0.3);
  //ロボット座標系における目標位置
  Orthogonal to = worldToLocal(targetList[iTarget], pos);
  //極座標系へ変換（tp.r 距離，tp.a 角度）
  Polar tp =orthogonalToPolar(to);
  //ロボット座標系におけるサブ目標位置
  Orthogonal so = worldToLocal(subTarget, pos);
  //極座標系へ変換（sp.r 距離，sp.a 角度）
  Polar sp = orthogonalToPolar(so);

  //状態遷移
  if (state == Goal) {
    //何もしない
  } else if (bum.right || bum.center || bum.left) {
    //いずれかのバンパに接触していれば
    contactTime = currentTime;
    state = AvoidBack;
  } else if (state == Move) {
    if (tp.r < 0.1) { //目標に近ければ
      if (iTarget < NumTarget-1) { //最後の目標でなければ
        iTarget++;
      } else { //最後の目標であれば
        state = Goal;
      }
    }
  } else if (state == AvoidBack) {
    if (currentTime - contactTime > 1) { //接触から一定時間経過していれば
      if (to.y > 0) { //目標が左側にあれば
        so.x = 0; so.y = +0.7; so.q = 0;
      } else { //目標が右側にあれば
        so.x = 0; so.y = -0.7; so.q = 0;
      }
      subTarget = localToWorld(so, pos); //サブ目標を世界座標系へ変換
      state = AvoidMove;
    }
  } else if (state == AvoidMove) {
    if (sp.r < 0.1) { //サブ目標が近ければ
      state = Move;
    }
  }

  //状態に応じた指令の決定
  if (state == Move) {
    if (abs(tp.a) > 0.1) { //正面になければ
      vel.v = 0;
      if (tp.a > 0) { //左にあれば
        vel.w = 0.5;
      } else { //右にあれば
        vel.w = -0.5;
      }
    } else { //正面にあれば
      vel.v = 0.2;
      vel.w = 0;
    }
  } else if (state == AvoidBack) {
    vel.v = -0.1;
    vel.w = 0;
  } else if (state == AvoidMove) {
    //サブ目標に十字を描く
    drawCross(subTarget.x, subTarget.y, 255, 0, 0, 0.2);
    if (abs(sp.a) > 0.1) { //正面になければ
      vel.v = 0;
      if (sp.a > 0) { //左にあれば
        vel.w = 0.5;
      } else { //右にあれば
        vel.w = -0.5;
      }
    } else { //正面にあれば
      vel.v = 0.2;
      vel.w = 0;
    }
  } else if (state == Goal) {
    vel.v = 0;
    vel.w = 0;
    drawString(1,2,36,255,0,0,"GOAL");
    if (goalTime == 0) { //初めてであれば
      goalTime = currentTime;
    }
    drawString(1,1.5,36,255,0,0,"%.1fsec",
      goalTime-startTime);
  }
  return vel;
}