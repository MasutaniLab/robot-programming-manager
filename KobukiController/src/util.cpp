//情報システム実験「ロボットプログラミング」
//2016/5/18 升谷 保博

#include "util.h"
#if defined(_WINDOWS) || defined(WIN32) //Windows
  #include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <mmsystem.h>

bool useQPC = false;//高精度カウンタ使用フラグ
LONGLONG QPFrequency;
LONGLONG basetime; 
#elif LINUX //Linux
  #include <termio.h>
  #include <unistd.h>
  struct termios originaltio;
  #include <sys/time.h>
  unsigned long basetime;  
#endif

#include <cstdlib>
#include <cmath>



int inkey()
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  if(_kbhit())
  {
    return _getch();
  }
  return -1;
#elif LINUX //Linux
  int r;
  unsigned char c;

  if ( read(0, &c, 1) == 1 ) 
  {
    r = c;
  } 
  else 
  {
    r = -1;
  }
  return r;
#endif
}


void getTimeInitialize()
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  if(QueryPerformanceFrequency((LARGE_INTEGER*)&QPFrequency))
  {
    if(QueryPerformanceCounter((LARGE_INTEGER*)&basetime))
    {
      useQPC = true;
    }
  }
  else basetime = timeGetTime();
#elif LINUX //Linux
  struct timeval now;

  gettimeofday(&now,NULL);
  basetime = now.tv_sec;
#endif
}

double getTime()
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  if(useQPC)
  {
    LONGLONG time;
    if(QueryPerformanceCounter((LARGE_INTEGER*)&time))
    {
      return (double)(time-basetime)/QPFrequency;
    }
  }
  return (double)(timeGetTime()-basetime)/1000.0;
#elif LINUX //Linux
  struct timeval now;

  gettimeofday(&now,NULL);
  return now.tv_sec - basetime + now.tv_usec*1e-6;
#endif
}
