//情報システム実験「ロボットプログラミング」
//2016/5/18 升谷 保博

#if defined(_WINDOWS) || defined(WIN32) //Windows
  //Windows
  #include <wineggx.h>
  #define FONTNAME "ＭＳゴシック"

  #ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES
  #endif

#elif LINUX //Linux
  #define _vsnprintf vsnprintf
  //C言語へのリンケージ解決
  extern "C"
  {
    #include "eggx.h"
  }
  #define FONTNAME "Courier"
#endif
#include "ise-robot.h"

int drawInitialize();
void draw(const Orthogonal &pos, const Bumper &bum, const Velocity &vel);
void drawTerminate();
int drawInkey();

void drawSetColor(int red,int green,int blue);
void drawGetColor(int &red,int &green,int &blue);
void drawPoint( double x, double y );
void drawPoint( double x, double y, int red, int green, int blue);
void drawCross( double x, double y, double size = 30);
void drawCross( double x, double y, int red, int green, int blue, double size = 30);
void drawLine(  double x1,double y1,double x2,double y2 );
void drawLine(  double x1,double y1,double x2,double y2, int red, int green, int blue);
void drawRectangle(double x, double y, double cx, double cy);
void drawRectangle(double x, double y, double cx, double cy, int red, int green, int blue);
void drawFillRectangle(double x, double y, double cx, double cy);
void drawFillRectangle(double x, double y, double cx, double cy, int red, int green, int blue);
void drawCircle(double x, double y, double r );
void drawCircle(double x, double y, double r, int red, int green, int blue);
void drawFillCircle(double x, double y, double r );
void drawFillCircle(double x, double y, double r, int red, int green, int blue);
void drawString(double x, double y, int size, const char *str, ... );
void drawString(double x, double y, int size,int red,int green,int blue,const char *str, ... );
