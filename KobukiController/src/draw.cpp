//���V�X�e�������u���{�b�g�v���O���~���O�v
//2016/6/29 ���J �۔�
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

//#define FONTNAME "�l�r�S�V�b�N"
#define GUI_FRAME_RATE (30)///< �`�摬�x(default=30)[fps]

#define MAX_DRAW_OBJECT (256) ///<1�t���[���ŕ`��\�ȍő�I�u�W�F�N�g��
#define POINT         (1) ///<�_
#define CROSS         (2) ///<�\��
#define LINE          (3) ///<��
#define RECTANGLE     (4) ///<�l�p�`
#define FILLRECTANGLE (5) ///<�l�p�`�i�h��Ԃ��j
#define CIRCLE        (6) ///<�~
#define FILLCIRCLE    (7) ///<�~�i�h��Ԃ��j
#define STRING        (8) ///<������

void drawGrid(int win, double xmin, double ymin, double xmax, double ymax, double grid);
//�O���[�o���萔�E�ϐ�
const double XMIN = -0.5;
const double XMAX = 3.5;
const double YMIN = -0.5;
const double YMAX = 4.5;
const double GRID = 0.5;
const double SCALE = 100;
const double RADIUS = 0.178;

bool gWindowEnable = false;    ///<�E�B���h�E��\�����邩�ǂ���?
int gRed   = 255;///<�`��F(��)
int gGreen = 128;///<�`��F(��)
int gBlue  = 255;///<�`��F(��)
double gDrawTime = 0;         ///<�`�掞��
double gDrawFrameTime = 0.015;///<�`�掞��

int win;
double keyTime, drawTime;

///
///@brief ���[�U�`��p�\����
///
struct DRAW_OBJECT
{
  int type;             ///<���
  double x1;            ///<x���W1
  double y1;            ///<y���W1
  double x2;            ///<x���W2
  double y2;            ///<y���W2
  double r;             ///<���a
  unsigned char red;    ///<�F�̐Ԑ���
  unsigned char green;  ///<�F�̗ΐ���
  unsigned char blue;   ///<�F�̐���
  string str;           ///<������
};

///
///@brief ���[�U�`��p�N���X
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
  vector<DRAW_OBJECT> m_object; ///<�`��I�u�W�F�N�g�̕���
  int m_num; ///<�I�u�W�F�N�g�̐�
  int m_max; ///<�I�u�W�F�N�g�̍ő吔
}gDrawData; ///<CDrawData�N���X�̑��I�u�W�F�N�g

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
  //�o���p
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
  //���x�w��
  newlinewidth(win, 2);
  newrgbcolor(win, 0, 0, 255);
  const double dt = 1;
  drawline(win, x, y, x+vel.v*dt*cos(q), y+vel.v*dt*sin(q));
  double dq = vel.w*dt;
  if ( dq >= 0.1 ) {// |dq|���ɒ[�ɏ������Ɖ~���\������Ă��܂����Ƃ�����̂�
    if ( dq > +2*M_PI ) dq = +2*M_PI;
    drawarc(win, x, y, RADIUS*0.8, RADIUS*0.8,RAD2DEG(q),RAD2DEG(q+dq),1);
  } else if ( dq <= -0.1 ){
    if ( dq < -2*M_PI ) dq = -2*M_PI;
    drawarc(win, x, y, RADIUS*0.8, RADIUS*0.8,RAD2DEG(q),RAD2DEG(q+dq),-1);
  }
  newlinewidth(win, 1);
  newrgbcolor(win, 0, 0, 0);

  //���[�U�[�`��  
  drawCommandProc();

  copylayer(win, 1, 0);
}

//�T�v�FEGGX�̏I��
//�����F�Ȃ�
//�߂�l�F�Ȃ�
void drawTerminate()
{
  gWindowEnable = false;
  gclose(win);
}

//�@�\�F���W���Ɗi�q��`���D
//�����Fwin �E�B���h�E�̔ԍ��C
//      xmin x�̍ŏ��l�Cymin y�̍ŏ��l�Cxman x�̍ő�l�Cymax y�̍ő�l�C
//      grid �i�q�̊Ԋu
//�߂�l�F�Ȃ�
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
///@brief     �R���X�g���N�^
///
CDrawData::CDrawData()
{
  m_num = 0;
  m_max = MAX_DRAW_OBJECT;
  m_object.reserve(MAX_DRAW_OBJECT);
}

///
///@brief �`�悷��_�̃f�[�^��o�^����
///@param[in] x x���W
///@param[in] y y���W
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷��~��̃f�[�^��o�^����
///@param[in] x x���W
///@param[in] y y���W
///@param[in] size ���@
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷����̃f�[�^��o�^����
///@param[in] x1 x���W1
///@param[in] y1 y���W1
///@param[in] x2 x���W2
///@param[in] y2 y���W2
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷��l�p�`�̃f�[�^��o�^����
///@param[in] x x���W
///@param[in] y y���W
///@param[in] cx x���@
///@param[in] cy y���@
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷��l�p�`�̃f�[�^��o�^����(�h��Ԃ�)
///@param[in] x x���W
///@param[in] y y���W
///@param[in] cx x���@
///@param[in] cy y���@
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷��~�̃f�[�^��o�^����
///@param[in] x x���W
///@param[in] y y���W
///@param[in] r ���a
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷��~�̃f�[�^��o�^����(�h��Ԃ�)
///@param[in] x x���W
///@param[in] y y���W
///@param[in] r ���a
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@return �Ȃ�
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
///@brief �`�悷�镶����̃f�[�^��o�^����
///@param[in] x x���W
///@param[in] y y���W
///@param[in] size ���@
///@param[in] rd ��
///@param[in] gr ��
///@param[in] bl ��
///@param[in] str ������
///@return �Ȃ�
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
///@brief  �`��p�̃f�[�^���擾����
///@param[in] id
///@param[out] data
///@retval true ����I��
///@retval false �ُ�I��
///
bool CDrawData::GetObject(int id,DRAW_OBJECT &data)
{
  int num = (int)m_object.size();
  if(num<=id||id<0)return false;
  data = m_object.at(id);
  return true;
}

///
///@brief  �N���[������
///@return �Ȃ�
///
void CDrawData::CleanData()
{
  m_object.clear();
  m_num = 0;
}

///
///@brief  �F�𒲐�����
///@param[in] color �F�v�f�̒l
///@return 0�`255�Ɏ��߂�����
///
inline unsigned char ChangeColor(int color)
{
  if(color<  0)return 0;
  if(color>255)return 255;
  return (unsigned char)color;
}

///
///@brief     �o�^����Ă���E�B���h�E�̕`��I�u�W�F�N�g���N���A����
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///
void drawClear()
{
  gDrawData.CleanData();
}

///
///@brief     �`��F��ݒ肷��
///@param[in] red   �ԐF ( 0 to 255 )
///@param[in] green �ΐF ( 0 to 255 )
///@param[in] blue  �F ( 0 to 255 )
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///
void drawSetColor(int red,int green,int blue)
{
  gRed   = ChangeColor(red);
  gGreen = ChangeColor(green);
  gBlue  = ChangeColor(blue);
}


///
///@brief      ���݂̕`��F���擾����
///@param[out] red   �ԐF ( 0 to 255 )
///@param[out] green �ΐF ( 0 to 255 )
///@param[out] blue  �F ( 0 to 255 )
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///
void drawGetColor(int &red,int &green,int &blue)
{
  red   = gRed;
  green = gGreen;
  blue  = gBlue;
}

///
///@brief     �_��`�悷��
///@param[in] x,y            �`�����W [mm]
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawPoint(double x,double y)
{
  gDrawData.SetPoint(x,y,gRed,gGreen,gBlue);
}

///
///@brief     �_��`�悷��
///@param[in] x,y            �`�����W [mm]
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawPoint( double x, double y , int red, int green, int blue)
{
  gDrawData.SetPoint(x,y,red,green,blue);
}

///
///@brief     �\���^�̃}�[�N��`�悷��
///@param[in] x,y            ���S���W [mm]
///@param[in] size           �\���̑傫��[mm](�f�t�H���g�ł�30)���ȗ���
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawCross(double x,double y,double size)
{
  gDrawData.SetCross(x,y,size,gRed,gGreen,gBlue);
}

///
///@brief     �\���^�̃}�[�N��`�悷��
///@param[in] x,y            ���S���W [mm]
///@param[in] size           �\���̑傫��[mm](�f�t�H���g�ł�30)���ȗ���
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawCross( double x, double y,int red, int green, int blue,double size)
{
  gDrawData.SetCross(x,y,size,red,green,blue);
}

///
///@brief     2�_�Ԃ̐�����`�悷��
///@param[in] x1,y1          �_1�̍��W [mm]
///@param[in] x2,y2          �_2�̍��W [mm]
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawLine(double x1,double y1,double x2,double y2)
{
  gDrawData.SetLine(x1,y1,x2,y2,gRed,gGreen,gBlue);
}

///
///@brief     2�_�Ԃ̐�����`�悷��
///@param[in] x1,y1          �_1�̍��W [mm]
///@param[in] x2,y2          �_2�̍��W [mm]
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawLine(  double x1,double y1,double x2,double y2, int red, int green, int blue)
{
  gDrawData.SetLine(x1,y1,x2,y2,red,green,blue);
}

///
///@brief     ���ɕ��s�Ȏl�p�`��`�悷��
///@param[in] x,y            ����̍��W [mm]
///@param[in] cx,cy          �l�p�`�̕� [mm]
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawRectangle(double x, double y, double cx, double cy)
{
  gDrawData.SetRectangle(x,y,cx,cy,gRed,gGreen,gBlue);
}

///
///@brief     ���ɕ��s�Ȏl�p�`��`�悷��
///@param[in] x,y            ����̍��W [mm]
///@param[in] cx,cy          �l�p�`�̕� [mm]
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  gDrawData.SetRectangle(x,y,cx,cy,red,green,blue);
}

///
///@brief     ���ɕ��s�Ȏl�p�`��`�悷��(������h��Ԃ�)
///@param[in] x,y            ����̍��W [mm]
///@param[in] cx,cy          �l�p�`�̕� [mm]
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawFillRectangle(double x, double y, double cx, double cy)
{
  gDrawData.SetFillRectangle(x,y,cx,cy,gRed,gGreen,gBlue);
}

///
///@brief     ���ɕ��s�Ȏl�p�`��`�悷��(������h��Ԃ�)
///@param[in] x,y            ����̍��W [mm]
///@param[in] cx,cy          �l�p�`�̕� [mm]
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawFillRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  gDrawData.SetFillRectangle(x,y,cx,cy,red,green,blue);
}

///
///@brief     �~��`�悷��
///@param[in] x,y            �~�̒��S���W [mm]
///@param[in] r              �~�̔��a
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawCircle(double x, double y, double r)
{
  gDrawData.SetCircle(x,y,r,gRed,gGreen,gBlue);
}


///
///@brief     �~��`�悷��
///@param[in] x,y            �~�̒��S���W [mm]
///@param[in] r              �~�̔��a
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawCircle(double x, double y, double r, int red, int green, int blue)
{
  gDrawData.SetCircle(x,y,r,red,green,blue);
}


///
///@brief     �~��`�悷��i�����h��Ԃ�)
///@param[in] x,y            �~�̒��S���W [mm]
///@param[in] r              �~�̔��a
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawFillCircle(double x, double y, double r )
{
  gDrawData.SetFillCircle(x,y,r,gRed,gGreen,gBlue);
}

///
///@brief     �~��`�悷��i�����h��Ԃ�)
///@param[in] x,y            �~�̒��S���W [mm]
///@param[in] r              �~�̔��a
///@param[in] red,green,blue �`��F(�͈� 0 to 255)
///@return �Ȃ�
///
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawFillCircle(double x, double y, double r, int red, int green, int blue)
{
  gDrawData.SetFillCircle(x,y,r,red,green,blue);
}

///
///@brief     �������`�悷��
///@param[in] x,y   �`�����W[mm]
///@param[in] size  �����T�C�Y(�W��14)
///@param[in] str   �`�敶����
///@return �Ȃ�
///
///- printf()�\���Ɠ����悤�ɕ�����`��ł���B
///- ���W�n�Ɋ֌W��������
///
void drawString(double x, double y, int size, const char *str, ... )
{
  assert(str);

  if(!size)size = 10;

  char buf[128];

  va_list argptr;             //�ϐ��������X�g
  va_start(argptr,str);       //initalize va_ function
#if defined(_WINDOWS) || defined(WIN32) //Windows
  int len = vsnprintf_s(buf,sizeof(buf),_TRUNCATE,str,argptr);//
#elif LINUX //Linux
  int len = vsnprintf(buf,sizeof(buf),str,argptr);//
#endif
  va_end(argptr);             //close     va_ function

  if (len==-1) {
    buf[sizeof(buf)-1] = '\0';//NULL�����t��
  } else {
    buf[len]                 = '\0';
  }

  gDrawData.SetString(x,y,size,gRed,gGreen,gBlue,buf);
}

/*
void drawtestmode(rx,ry,bx,by);
*/

///
///@brief     �������`�悷��
///@param[in] x,y   �`�����W[mm]
///@param[in] size  �����T�C�Y(�W��14)
///@param[in] red   �����F(�Ԑ���)
///@param[in] green �����F(�ΐ���)
///@param[in] blue  �����F(����)
///@param[in] str   �`�敶����
///@return �Ȃ�
///
///- printf()�\���Ɠ����悤�ɕ�����`��ł���
///- ���W�n�Ɋ֌W��������
///- �`��F�̐ݒ��1��i�Ăяo�������j�����L��
///
void drawString(double x, double y, int size,int red,int green,int blue, const char *str, ... )
{
  assert(str);

  if(!size)size = 10;

  char buf[128];

  va_list argptr;             //�ϐ��������X�g
  va_start(argptr,str);       //initalize va_ function
#if defined(_WINDOWS) || defined(WIN32) //Windows
  int len = vsnprintf_s(buf,sizeof(buf),_TRUNCATE,str,argptr);//
#elif LINUX //Linux
  int len = vsnprintf(buf,sizeof(buf),str,argptr);//
#endif
  va_end(argptr);             //close     va_ function

  if (len==-1) {
    buf[sizeof(buf)-1] = '\0';//NULL�����t��
  } else {
    buf[len]                 = '\0';
  }

  gDrawData.SetString(x,y,size,red,green,blue,buf);
}



///
///@brief  ���[�U�[�`��
///@return �Ȃ�
///
void drawCommandProc()
{
  //���[�U�[�`��  
  int i=0;
  DRAW_OBJECT dobj;
  while(1)
  {
    if(!gDrawData.GetObject(i,dobj))break;
    newrgbcolor(win,dobj.red,dobj.green,dobj.blue);
    switch(dobj.type)
    {
    case POINT:  //�_�`��
      pset(win,(float)dobj.x1,(float)dobj.y1);
      break;
    case CROSS:  //�~��`��
      line(win, (float)dobj.x1,(float)(dobj.y1-dobj.y2), PENUP);
      line(win, (float)dobj.x1,(float)(dobj.y1+dobj.y2), PENDOWN);
      line(win, (float)(dobj.x1-dobj.x2),(float)dobj.y1, PENUP);
      line(win, (float)(dobj.x1+dobj.x2),(float)dobj.y1, PENDOWN);
      break;
    case LINE:   //���`��
      line(win,(float)dobj.x1,(float)dobj.y1,PENUP);
      line(win,(float)dobj.x2,(float)dobj.y2,PENDOWN);
      break;
    case RECTANGLE:   //�l�p�`
      drawrect(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.x2,(float)dobj.y2);
      break;
    case FILLRECTANGLE://�l�p�`(�h��Ԃ�)
      fillrect(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.x2,(float)dobj.y2);
      break;
    case CIRCLE:       //�~�`��
      circle(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.r,(float)dobj.r);
      break;
    case FILLCIRCLE:   //�~�`��(�h��Ԃ�)
      fillarc(win,(float)dobj.x1,(float)dobj.y1,(float)dobj.r,(float)dobj.r, 0.0f,360.0f,1);
      break;
    case STRING: //������`��
      drawstr(win,(float)dobj.x1,(float)dobj.y1,(int)dobj.r,0,dobj.str.c_str());
    }
    i++;
  }
  //�N���A
  gDrawData.CleanData();
}
