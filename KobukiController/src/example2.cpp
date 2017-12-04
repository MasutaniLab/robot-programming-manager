//���V�X�e�������u���{�b�g�v���O���~���O�v
//��v���O����2: �ڕW�ւ̈ړ��{��Q�����
//2016/6/29 ���J �۔�
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

//�T�v�F�s������̂��߂̏����ݒ�
//�����F�Ȃ�
//�߂�l�F�Ȃ�
void setup()
{
  iTarget = 0;
  pause = true;
  startTime = goalTime = 0;
  state = Move;
  contactTime = 0;
  subTarget = targetList[0];
}

//�T�v�F�s������̂��߂̖���̏���
//�����Fpos ���݈ʒu�Cbum �o���p�Z���T�̏�ԁCkey �L�[����
//�߂�l�F���x�w��
Velocity loop(const Orthogonal &pos, const Bumper &bum, int key)
{
  Velocity vel;
  double currentTime = getTime(); //���݂̌o�ߎ��Ԃ��L��
  if (key >= 0) { //�L�[��������Ă����
    if (pause && key == 'r') { //��~���Łur�v�ł����
      pause = false;
      if (startTime == 0) { //���߂Ăł����
        startTime = currentTime;
      }
    } else { //���쒆���ur�v�łȂ����
      pause = true;
    }
  }
  if (pause) { //��~���ł����
    drawString(0,2,36,255,0,0,"Press 'r' key to start");
    vel.v = 0;
    vel.w = 0;
    return vel;
  }
  //�ڕW�ɏ\����`��
  drawCross(targetList[iTarget].x, targetList[iTarget].y, 255, 0, 0, 0.3);
  //���{�b�g���W�n�ɂ�����ڕW�ʒu
  Orthogonal to = worldToLocal(targetList[iTarget], pos);
  //�ɍ��W�n�֕ϊ��itp.r �����Ctp.a �p�x�j
  Polar tp =orthogonalToPolar(to);
  //���{�b�g���W�n�ɂ�����T�u�ڕW�ʒu
  Orthogonal so = worldToLocal(subTarget, pos);
  //�ɍ��W�n�֕ϊ��isp.r �����Csp.a �p�x�j
  Polar sp = orthogonalToPolar(so);

  //��ԑJ��
  if (state == Goal) {
    //�������Ȃ�
  } else if (bum.right || bum.center || bum.left) {
    //�����ꂩ�̃o���p�ɐڐG���Ă����
    contactTime = currentTime;
    state = AvoidBack;
  } else if (state == Move) {
    if (tp.r < 0.1) { //�ڕW�ɋ߂����
      if (iTarget < NumTarget-1) { //�Ō�̖ڕW�łȂ����
        iTarget++;
      } else { //�Ō�̖ڕW�ł����
        state = Goal;
      }
    }
  } else if (state == AvoidBack) {
    if (currentTime - contactTime > 1) { //�ڐG�����莞�Ԍo�߂��Ă����
      if (to.y > 0) { //�ڕW�������ɂ����
        so.x = 0; so.y = +0.7; so.q = 0;
      } else { //�ڕW���E���ɂ����
        so.x = 0; so.y = -0.7; so.q = 0;
      }
      subTarget = localToWorld(so, pos); //�T�u�ڕW�𐢊E���W�n�֕ϊ�
      state = AvoidMove;
    }
  } else if (state == AvoidMove) {
    if (sp.r < 0.1) { //�T�u�ڕW���߂����
      state = Move;
    }
  }

  //��Ԃɉ������w�߂̌���
  if (state == Move) {
    if (abs(tp.a) > 0.1) { //���ʂɂȂ����
      vel.v = 0;
      if (tp.a > 0) { //���ɂ����
        vel.w = 0.5;
      } else { //�E�ɂ����
        vel.w = -0.5;
      }
    } else { //���ʂɂ����
      vel.v = 0.2;
      vel.w = 0;
    }
  } else if (state == AvoidBack) {
    vel.v = -0.1;
    vel.w = 0;
  } else if (state == AvoidMove) {
    //�T�u�ڕW�ɏ\����`��
    drawCross(subTarget.x, subTarget.y, 255, 0, 0, 0.2);
    if (abs(sp.a) > 0.1) { //���ʂɂȂ����
      vel.v = 0;
      if (sp.a > 0) { //���ɂ����
        vel.w = 0.5;
      } else { //�E�ɂ����
        vel.w = -0.5;
      }
    } else { //���ʂɂ����
      vel.v = 0.2;
      vel.w = 0;
    }
  } else if (state == Goal) {
    vel.v = 0;
    vel.w = 0;
    drawString(1,2,36,255,0,0,"GOAL");
    if (goalTime == 0) { //���߂Ăł����
      goalTime = currentTime;
    }
    drawString(1,1.5,36,255,0,0,"%.1fsec",
      goalTime-startTime);
  }
  return vel;
}