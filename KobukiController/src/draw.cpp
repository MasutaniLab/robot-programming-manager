//情報システム実験「ロボットプログラミング」
//2016/6/29 升谷 保博
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
#include <cstdio>
#include <stdarg.h>
#include "util.h"
#include "draw.h"
using namespace std;

//#define FONTNAME "ＭＳゴシック"
#define GUI_FRAME_RATE (30)///< 描画速度(default=30)[fps]

#define MAX_DRAW_OBJECT (256) ///<1フレームで描画可能な最大オブジェクト数
#define POINT         (1) ///<点
#define CROSS         (2) ///<十時
#define LINE          (3) ///<線
#define RECTANGLE     (4) ///<四角形
#define FILLRECTANGLE (5) ///<四角形（塗りつぶし）
#define CIRCLE        (6) ///<円
#define FILLCIRCLE    (7) ///<円（塗りつぶし）
#define STRING        (8) ///<文字列

void drawGrid(int win, double xmin, double ymin, double xmax, double ymax, double grid);
//グローバル定数・変数
const double XMIN = -0.5;
const double XMAX = 3.5;
const double YMIN = -0.5;
const double YMAX = 4.5;
const double GRID = 0.5;
const double SCALE = 100;
const double RADIUS = 0.178;

bool gWindowEnable = false;    ///<ウィンドウを表示するかどうか?
int gRed   = 255;///<描画色(赤)
int gGreen = 128;///<描画色(緑)
int gBlue  = 255;///<描画色(青)
double gDrawTime = 0;         ///<描画時間
double gDrawFrameTime = 0.015;///<描画時間

int win;
double keyTime, drawTime;

///
///@brief ユーザ描画用構造体
///
struct DRAW_OBJECT
{
  int type;             ///<種類
  double x1;            ///<x座標1
  double y1;            ///<y座標1
  double x2;            ///<x座標2
  double y2;            ///<y座標2
  double r;             ///<半径
  unsigned char red;    ///<色の赤成分
  unsigned char green;  ///<色の緑成分
  unsigned char blue;   ///<色の青成分
  string str;           ///<文字列
};

///
///@brief ユーザ描画用クラス
///
class CDrawData
{
public:
  CDrawData();

  void SetPoint( double x, double y, unsigned rd,unsigned gr,unsigned bl);
  void SetCross( double x, double y, double size,unsigned rd,unsigned gr,unsigned bl);
  void SetLine(  double x1,double y1,double x2,double y2, unsigned rd,unsigned gr,unsigned bl);
  void SetRectangle(    double x, double y, double cx,double cy,unsigned rd,unsigned gr,unsigned bl);
  void SetFillRectangle(double x, double y, double cx,double cy,unsigned rd,unsigned gr,unsigned bl);
  void SetCircle(    double x, double y, double r,unsigned rd,unsigned gr,unsigned bl);
  void SetFillCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl);
  void SetString(    double x, double y, int size,unsigned rd,unsigned gr,unsigned bl,char *str);

  bool GetObject(int id,DRAW_OBJECT &data);
  void CleanData();
private:
  vector<DRAW_OBJECT> m_object; ///<描画オブジェクトの並び
  int m_num; ///<オブジェクトの数
  int m_max; ///<オブジェクトの最大数
}gDrawData; ///<CDrawDataクラスの大域オブジェクト

void drawCommandProc();


int drawInitialize()
{
  if(gWindowEnable)return win;

  win = gopen(int(SCALE*(XMAX-XMIN)), int(SCALE*(YMAX-YMIN)));
  if(win==-1) {
    cerr << "error - initialize window" << endl;
    return -1;
  }

  window(win, XMIN, YMIN, XMAX, YMAX);
  drawGrid(win, XMIN, YMIN, XMAX, YMAX, GRID);
  layer(win, 0, 1);
  gsetnonblock(ENABLE);
  keyTime = drawTime = getTime();
  gsetfontset(win,FONTNAME);
  gWindowEnable    = true;
  return win;
}

void draw(const Orthogonal &pos, const Bumper &bum, const Velocity &vel)
{
  double currentTime = getTime();
  if (currentTime-drawTime < 0.0333 ) {
    gDrawData.CleanData();
    return;
  }
  drawTime = currentTime;
  const double &x = pos.x;
  const double &y = pos.y;
  const double &q = pos.q;
  drawGrid(win, XMIN, YMIN, XMAX, YMAX, GRID);
  circle(win, x, y, RADIUS, RADIUS);
  //バンパ
  newlinewidth(win, 2);
  if (bum.right) {
    newrgbcolor(win, 255, 0, 0);
  } else {
    newrgbcolor(win, 0, 255, 0);
  }
  drawarc(win, x, y, RADIUS*1.2, RADIUS*1.2,
    RAD2DEG(q-M_PI/2+0.1),RAD2DEG(q-M_PI/6-0.1),1);
  if (bum.center) {
    newrgbcolor(win, 255, 0, 0);
  } else {
    newrgbcolor(win, 0, 255, 0);
  }
  drawarc(win, x, y, RADIUS*1.2, RADIUS*1.2,
    RAD2DEG(q-M_PI/6+0.1),RAD2DEG(q+M_PI/6-0.1),1);
  if (bum.left) {
    newrgbcolor(win, 255, 0, 0);
  } else {
    newrgbcolor(win, 0, 255, 0);
  }
  drawarc(win, x, y, RADIUS*1.2, RADIUS*1.2,
    RAD2DEG(q+M_PI/6+0.1),RAD2DEG(q+M_PI/2-0.1),1);
  //速度指令
  newlinewidth(win, 2);
  newrgbcolor(win, 0, 0, 255);
  const double dt = 1;
  drawline(win, x, y, x+vel.v*dt*cos(q), y+vel.v*dt*sin(q));
  double dq = vel.w*dt;
  if ( dq >= 0.1 ) {// |dq|が極端に小さいと円が表示されてしまうことがあるので
    if ( dq > +2*M_PI ) dq = +2*M_PI;
    drawarc(win, x, y, RADIUS*0.8, RADIUS*0.8,RAD2DEG(q),RAD2DEG(q+dq),1);
  } else if ( dq <= -0.1 ){
    if ( dq < -2*M_PI ) dq = -2*M_PI;
    drawarc(win, x, y, RADIUS*0.8, RADIUS*0.8,RAD2DEG(q),RAD2DEG(q+dq),-1);
  }
  newlinewidth(win, 1);
  newrgbcolor(win, 0, 0, 0);

  //ユーザー描画  
  drawCommandProc();

  copylayer(win, 1, 0);
}

//概要：EGGXの終了
//引数：なし
//戻り値：なし
void drawTerminate()
{
  gWindowEnable = false;
  gclose(win);
}

//機能：座標軸と格子を描く．
//引数：win ウィンドウの番号，
//      xmin xの最小値，ymin yの最小値，xman xの最大値，ymax yの最大値，
//      grid 格子の間隔
//戻り値：なし
void drawGrid(int win, 
              double xmin, double ymin, double xmax, double ymax, 
              double grid) 
{
    //gsetbgcolorrgb(win,255,255,255);
    gsetbgcolor(win,"white");
    gclr(win);
    newrgbcolor(win,0,0,0);
    newlinewidth(win, 1);
    newlinestyle(win,LineOnOffDash);
    for (double x = xmin; x <= xmax; x+=grid) {
        drawline(win, x, ymin, x, ymax);
    }
    for (double y = ymin; y <= ymax; y+=grid) {
        drawline(win, xmin, y, xmax, y);
    }
    newlinestyle(win, LineSolid);
    drawline(win, 0, ymin, 0, ymax);
    drawline(win, xmin, 0, xmax, 0);
}

int drawInkey()
{
  double currentTime = getTime();
  if (currentTime-keyTime < 0.1 ) {
    return -1;
  }
  keyTime = currentTime;
  return ggetch();
}

// =====================================================================

///
///@brief     コンストラクタ
///
CDrawData::CDrawData()
{
  m_num = 0;
  m_max = MAX_DRAW_OBJECT;
  m_object.reserve(MAX_DRAW_OBJECT);
}

///
///@brief 描画する点のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetPoint(double x, double y, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = POINT;
  data.x1    = x;
  data.y1    = y;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する×印のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] size 寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetCross(double x, double y,double size, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = CROSS;
  data.x1    = x;
  data.y1    = y;
  data.x2    = size;
  data.y2    = size;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する線のデータを登録する
///@param[in] x1 x座標1
///@param[in] y1 y座標1
///@param[in] x2 x座標2
///@param[in] y2 y座標2
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetLine(double x1,double y1,double x2,double y2, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = LINE;
  data.x1    = x1;
  data.y1    = y1;
  data.x2    = x2;
  data.y2    = y2;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する四角形のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] cx x寸法
///@param[in] cy y寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetRectangle(    double x,double y,double cx,double cy,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = RECTANGLE;
  data.x1    = x;
  data.y1    = y;
  data.x2    = cx;
  data.y2    = cy;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する四角形のデータを登録する(塗りつぶし)
///@param[in] x x座標
///@param[in] y y座標
///@param[in] cx x寸法
///@param[in] cy y寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetFillRectangle(double x,double y,double cx,double cy,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = FILLRECTANGLE;
  data.x1    = x;
  data.y1    = y;
  data.x2    = cx;
  data.y2    = cy;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;

}

///
///@brief 描画する円のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] r 半径
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = CIRCLE;
  data.x1    = x;
  data.y1    = y;
  data.r     = r;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する円のデータを登録する(塗りつぶし)
///@param[in] x x座標
///@param[in] y y座標
///@param[in] r 半径
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetFillCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = FILLCIRCLE;
  data.x1    = x;
  data.y1    = y;
  data.r     = r;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する文字列のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] size 寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@param[in] str 文字列
///@return なし
///
void CDrawData::SetString(double x, double y, int size, unsigned rd,unsigned gr,unsigned bl,char *str)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = STRING;
  data.x1    = x;
  data.y1    = y;
  data.r     = size;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  data.str   = str;
  m_object.push_back(data);
  m_num++;
}

///
///@brief  描画用のデータを取得する
///@param[in] id
///@param[out] data
///@retval true 正常終了
///@retval false 異常終了
///
bool CDrawData::GetObject(int id,DRAW_OBJECT &data)
{
  int num = (int)m_object.size();
  if(num<=id||id<0)return false;
  data = m_object.at(id);
  return true;
}

///
///@brief  クリーンする
///@return なし
///
void CDrawData::CleanData()
{
  m_object.clear();
  m_num = 0;
}

///
///@brief  色を調整する
///@param[in] color 色要素の値
///@return 0～255に収めた結果
///
inline unsigned char ChangeColor(int color)
{
  if(color<  0)return 0;
  if(color>255)return 255;
  return (unsigned char)color;
}

///
///@brief     登録されているウィンドウの描画オブジェクトをクリアする
///@return なし
///
///- 座標系に関係無く共通
///
void drawClear()
{
  gDrawData.CleanData();
}

///
///@brief     描画色を設定する
///@param[in] red   赤色 ( 0 to 255 )
///@param[in] green 緑色 ( 0 to 255 )
///@param[in] blue  青色 ( 0 to 255 )
///@return なし
///
///- 座標系に関係無く共通
///
void drawSetColor(int red,int green,int blue)
{
  gRed   = ChangeColor(red);
  gGreen = ChangeColor(green);
  gBlue  = ChangeColor(blue);
}


///
///@brief      現在の描画色を取得する
///@param[out] red   赤色 ( 0 to 255 )
///@param[out] green 緑色 ( 0 to 255 )
///@param[out] blue  青色 ( 0 to 255 )
///@return なし
///
///- 座標系に関係無く共通
///
void drawGetColor(int &red,int &green,int &blue)
{
  red   = gRed;
  green = gGreen;
  blue  = gBlue;
}

///
///@brief     点を描画する
///@param[in] x,y            描画先座標 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawPoint(double x,double y)
{
  gDrawData.SetPoint(x,y,gRed,gGreen,gBlue);
}

///
///@brief     点を描画する
///@param[in] x,y            描画先座標 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawPoint( double x, double y , int red, int green, int blue)
{
  gDrawData.SetPoint(x,y,red,green,blue);
}

///
///@brief     十字型のマークを描画する
///@param[in] x,y            中心座標 [mm]
///@param[in] size           十字の大きさ[mm](デフォルトでは30)＊省略可
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCross(double x,double y,double size)
{
  gDrawData.SetCross(x,y,size,gRed,gGreen,gBlue);
}

///
///@brief     十字型のマークを描画する
///@param[in] x,y            中心座標 [mm]
///@param[in] size           十字の大きさ[mm](デフォルトでは30)＊省略可
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCross( double x, double y,int red, int green, int blue,double size)
{
  gDrawData.SetCross(x,y,size,red,green,blue);
}

///
///@brief     2点間の線分を描画する
///@param[in] x1,y1          点1の座標 [mm]
///@param[in] x2,y2          点2の座標 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawLine(double x1,double y1,double x2,double y2)
{
  gDrawData.SetLine(x1,y1,x2,y2,gRed,gGreen,gBlue);
}

///
///@brief     2点間の線分を描画する
///@param[in] x1,y1          点1の座標 [mm]
///@param[in] x2,y2          点2の座標 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawLine(  double x1,double y1,double x2,double y2, int red, int green, int blue)
{
  gDrawData.SetLine(x1,y1,x2,y2,red,green,blue);
}

///
///@brief     軸に平行な四角形を描画する
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawRectangle(double x, double y, double cx, double cy)
{
  gDrawData.SetRectangle(x,y,cx,cy,gRed,gGreen,gBlue);
}

///
///@brief     軸に平行な四角形を描画する
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  gDrawData.SetRectangle(x,y,cx,cy,red,green,blue);
}

///
///@brief     軸に平行な四角形を描画する(内部を塗りつぶす)
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillRectangle(double x, double y, double cx, double cy)
{
  gDrawData.SetFillRectangle(x,y,cx,cy,gRed,gGreen,gBlue);
}

///
///@brief     軸に平行な四角形を描画する(内部を塗りつぶす)
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  gDrawData.SetFillRectangle(x,y,cx,cy,red,green,blue);
}

///
///@brief     円を描画する
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCircle(double x, double y, double r)
{
  gDrawData.SetCircle(x,y,r,gRed,gGreen,gBlue);
}


///
///@brief     円を描画する
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCircle(double x, double y, double r, int red, int green, int blue)
{
  gDrawData.SetCircle(x,y,r,red,green,blue);
}


///
///@brief     円を描画する（内部塗りつぶし)
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillCircle(double x, double y, double r )
{
  gDrawData.SetFillCircle(x,y,r,gRed,gGreen,gBlue);
}

///
///@brief     円を描画する（内部塗りつぶし)
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillCircle(double x, double y, double r, int red, int green, int blue)
{
  gDrawData.SetFillCircle(x,y,r,red,green,blue);
}

///
///@brief     文字列を描画する
///@param[in] x,y   描画先座標[mm]
///@param[in] size  文字サイズ(標準14)
///@param[in] str   描画文字列
///@return なし
///
///- printf()構文と同じように文字を描画できる。
///- 座標系に関係無く共通
///
void drawString(double x, double y, int size, const char *str, ... )
{
  assert(str);

  if(!size)size = 10;

  char buf[128];

  va_list argptr;             //可変数引数リスト
  va_start(argptr,str);       //initalize va_ function
#if defined(_WINDOWS) || defined(WIN32) //Windows
  int len = vsnprintf_s(buf,sizeof(buf),_TRUNCATE,str,argptr);//
#elif LINUX //Linux
  int len = vsnprintf(buf,sizeof(buf),str,argptr);//
#endif
  va_end(argptr);             //close     va_ function

  if (len==-1) {
    buf[sizeof(buf)-1] = '\0';//NULL文字付加
  } else {
    buf[len]                 = '\0';
  }

  gDrawData.SetString(x,y,size,gRed,gGreen,gBlue,buf);
}

/*
void drawtestmode(rx,ry,bx,by);
*/

///
///@brief     文字列を描画する
///@param[in] x,y   描画先座標[mm]
///@param[in] size  文字サイズ(標準14)
///@param[in] red   文字色(赤成分)
///@param[in] green 文字色(緑成分)
///@param[in] blue  文字色(青成分)
///@param[in] str   描画文字列
///@return なし
///
///- printf()構文と同じように文字を描画できる
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawString(double x, double y, int size,int red,int green,int blue, const char *str, ... )
{
  assert(str);

  if(!size)size = 10;

  char buf[128];

  va_list argptr;             //可変数引数リスト
  va_start(argptr,str);       //initalize va_ function
#if defined(_WINDOWS) || defined(WIN32) //Windows
  int len = vsnprintf_s(buf,sizeof(buf),_TRUNCATE,str,argptr);//
#elif LINUX //Linux
  int len = vsnprintf(buf,sizeof(buf),str,argptr);//
#endif
  va_end(argptr);             //close     va_ function

  if (len==-1) {
    buf[sizeof(buf)-1] = '\0';//NULL文字付加
  } else {
    buf[len]                 = '\0';
  }

  gDrawData.SetString(x,y,size,red,green,blue,buf);
}



///
///@brief  ユーザー描画
///@return なし
///
void drawCommandProc()
{
  //ユーザー描画  
  int i=0;
  DRAW_OBJECT dobj;
  while(1)
  {
    if(!gDrawData.GetObject(i,dobj))break;
    newrgbcolor(win,dobj.red,dobj.green,dobj.blue);
    switch(dobj.type)
    {
    case POINT:  //点描画
      pset(win,(float)dobj.x1,(float)dobj.y1);
      break;
    case CROSS:  //×印描画
      line(win, (float)dobj.x1,(float)(dobj.y1-dobj.y2), PENUP);
      line(win, (float)dobj.x1,(float)(dobj.y1+dobj.y2), PENDOWN);
      line(win, (float)(dobj.x1-dobj.x2),(float)dobj.y1, PENUP);
      line(win, (float)(dobj.x1+dobj.x2),(float)dobj.y1, PENDOWN);
      break;
    case LINE:   //線描画
      line(win,(float)dobj.x1,(float)dobj.y1,PENUP);
      line(win,(float)dobj.x2,(float)dobj.y2,PENDOWN);
      break;
    case RECTANGLE:   //四角形
      drawrect(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.x2,(float)dobj.y2);
      break;
    case FILLRECTANGLE://四角形(塗りつぶし)
      fillrect(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.x2,(float)dobj.y2);
      break;
    case CIRCLE:       //円描画
      circle(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.r,(float)dobj.r);
      break;
    case FILLCIRCLE:   //円描画(塗りつぶし)
      fillarc(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.r,(float)dobj.r, 0.0f,360.0f,1);
      break;
    case STRING: //文字列描画
      drawstr(win,(float)dobj.x1,(float)dobj.y1,(int)dobj.r,0,dobj.str.c_str());
    }
    i++;
  }
  //クリア
  gDrawData.CleanData();
}
