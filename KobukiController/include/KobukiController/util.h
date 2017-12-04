//情報システム実験「ロボットプログラミング」
//2016/5/18 升谷 保博
#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

int inkey();
void getTimeInitialize();
double getTime();

/**
 * @brief     角度を [degree] 単位から [radian] 単位に変換する。
 * @ingroup   util
 * @def       DEG2RAD(x)
 * @param[in] x 角度[degree]
 * @return    角度[radian]
**/
#define DEG2RAD(x) ((double)(x)*M_PI/180.0)

/**
 * @brief     角度を [radian] 単位から [degree] 単位に変換する。
 * @ingroup   util
 * @def       RAD2DEG(x)
 * @param[in] x 角度[radian]
 * @return    角度[degree]
**/
#define RAD2DEG(x) ((double)(x)*180.0/M_PI)

/**
 * @brief     角度を正規化する
 * @ingroup   util
 * @param[in] x 角度[radian]
 * @return    -Pi から +Pi
 * @attention
 *  角度を-Piから+Piの間に正規化する。
**/
inline double normalizeAngle(double x)
{
  if      (x<-M_PI) x += M_PI*2*(double)((int)(-x/(2*M_PI)+0.5));
  else if (x> M_PI) x -= M_PI*2*(double)((int)( x/(2*M_PI)+0.5));
  return x;
}


/**
 * @brief     引数の符号（プラスマイナス）を返す
 * @ingroup   util
 * @param[in] x 数値
 * @retval    -1 引数の値が0より小さい場合
 * @retval    0  引数が0の時
 * @retval    +1 引数の値が0より大きい場合
**/
inline int sign( double x)
{
  if     ( x < 0 ) return -1;
  else if( x > 0 ) return  1;
  return 0;
}
