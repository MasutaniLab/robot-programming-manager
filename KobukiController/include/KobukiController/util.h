//���V�X�e�������u���{�b�g�v���O���~���O�v
//2016/5/18 ���J �۔�
#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

int inkey();
void getTimeInitialize();
double getTime();

/**
 * @brief     �p�x�� [degree] �P�ʂ��� [radian] �P�ʂɕϊ�����B
 * @ingroup   util
 * @def       DEG2RAD(x)
 * @param[in] x �p�x[degree]
 * @return    �p�x[radian]
**/
#define DEG2RAD(x) ((double)(x)*M_PI/180.0)

/**
 * @brief     �p�x�� [radian] �P�ʂ��� [degree] �P�ʂɕϊ�����B
 * @ingroup   util
 * @def       RAD2DEG(x)
 * @param[in] x �p�x[radian]
 * @return    �p�x[degree]
**/
#define RAD2DEG(x) ((double)(x)*180.0/M_PI)

/**
 * @brief     �p�x�𐳋K������
 * @ingroup   util
 * @param[in] x �p�x[radian]
 * @return    -Pi ���� +Pi
 * @attention
 *  �p�x��-Pi����+Pi�̊Ԃɐ��K������B
**/
inline double normalizeAngle(double x)
{
  if      (x<-M_PI) x += M_PI*2*(double)((int)(-x/(2*M_PI)+0.5));
  else if (x> M_PI) x -= M_PI*2*(double)((int)( x/(2*M_PI)+0.5));
  return x;
}


/**
 * @brief     �����̕����i�v���X�}�C�i�X�j��Ԃ�
 * @ingroup   util
 * @param[in] x ���l
 * @retval    -1 �����̒l��0��菬�����ꍇ
 * @retval    0  ������0�̎�
 * @retval    +1 �����̒l��0���傫���ꍇ
**/
inline int sign( double x)
{
  if     ( x < 0 ) return -1;
  else if( x > 0 ) return  1;
  return 0;
}
